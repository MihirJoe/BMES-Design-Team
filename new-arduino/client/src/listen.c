#include "listen.h"

#include "support.h"

#include <adapt/proto.h>

#include <pthread.h>
#include <unistd.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// NOTE: for a baud of 115200
//       (the maximum we will support),
//       the theoretical maximum number of bytes
//       the Arduino can transmit in one decisecond (100 ms)
//       is exactly 1440.
#define LISTEN_BUF_SIZE 1440

#define LISTEN_CTX_INIT                                                        \
  ((struct listen_ctx){.conn = {.busy_flag = ATOMIC_FLAG_INIT},                \
                       .send_ctx = {.continue_flag = ATOMIC_FLAG_INIT},        \
                       .recv_ctx = {.continue_flag = ATOMIC_FLAG_INIT}})

struct incoming {
  unsigned char data_buf[LISTEN_BUF_SIZE];
  size_t data_len;
};

struct conn {
  struct incoming incoming;
  unsigned char read_buf[LISTEN_BUF_SIZE];
  atomic_bool incoming_out_for_delivery;
  atomic_flag busy_flag;
  pthread_mutex_t incoming_lock;
  pthread_cond_t recv_has_work;
};

struct send_ctx {
  struct conn *conn;
  int serial_fd;
  atomic_flag continue_flag;
};

struct monitor_ctx {
  struct conn *conn;
  atomic_flag *continue_flag;
};

struct recv_ctx {
  struct conn *conn;
  adptc_listen_monitor mon;
  void *mon_user_ctx;
  atomic_flag continue_flag;
};

struct listen_ctx {
  struct conn conn;
  struct send_ctx send_ctx;
  struct recv_ctx recv_ctx;
  pthread_t send_thread;
  pthread_t recv_thread;
};

adptc_listen adptc_listen_create(void) {
  struct listen_ctx *const ctx = malloc(sizeof(struct listen_ctx));
  if (!ctx)
    return NULL;

  *ctx = LISTEN_CTX_INIT;
  ctx->send_ctx.conn = &ctx->conn;
  ctx->recv_ctx.conn = &ctx->conn;

  return ctx;
}

void adptc_listen_destroy(adptc_listen lhnd) {
  struct listen_ctx *const ctx = lhnd;
  assert(ctx);

  free(ctx);
}

static void *send_routine(void *const thread_arg) {
  struct send_ctx *const ctx = thread_arg;
  assert(ctx);
  assert(ctx->conn);

  // NOTE: this is a finite state machine.

  enum state {
    state_read,
    state_wait,
    state_deliver,
  };

  int pthread_res;

  enum state state = state_read;
  ssize_t read_res;

  // NOTE: in-between states,
  //       check the 'continue' flag.
  //       When it is cleared, return.
  while (atomic_flag_test_and_set(&ctx->continue_flag)) {
    switch (state) {
    case state_read:
      // NOTE: we don't read directly into the 'incoming' structure
      //       because we plan to begin the next read
      //       while the receiver thread
      //       is processing the previous incoming data.
      read_res = read(ctx->serial_fd, ctx->conn->read_buf, LISTEN_BUF_SIZE);
      if (read_res <= 0)
        // TODO
        read_res = 0;

      state = state_wait;
      break;

    case state_wait:
      // Wait until the receiver thread is done using the 'incoming' structure
      // so that we can copy the read data into it.
      // NOTE: because the default state of the 'busy' flag is set,
      //       on the first iteration of this loop,
      //       we wait until the receiver thread
      //       calls adptc_listen_accept_incoming.
      //       This gives us confidence
      //       that the receiver thread
      //       will receive the condition variable signal.
      // NOTE: this is basically a spinlock.
      //       Condition variables are costly
      //       and we don't expect to be here very long.
      if (!atomic_flag_test_and_set(&ctx->conn->busy_flag))
        state = state_deliver;

      break;

    case state_deliver:;
      // Copy the read data to the 'incoming' structure.
      // NOTE: this structure is protected not by a lock
      //       but by the 'busy' flag,
      //       which, when cleared,
      //       indicates that the receiver thread
      //       is not using the 'incoming' structure.
      size_t const read_len = read_res;
      memcpy(ctx->conn->incoming.data_buf, ctx->conn->read_buf, read_len);
      ctx->conn->incoming.data_len = read_len;

      // NOTE: Acquiring this lock
      //       is part of the song and dance of using condition variables.
      pthread_res = pthread_mutex_lock(&ctx->conn->incoming_lock);
      if (pthread_res != 0)
        adptc_support_todo;

      // Indicate to the receiver thread
      // that incoming serial data is available.
      atomic_store(&ctx->conn->incoming_out_for_delivery, true);

      // Release the lock
      // so that the receiver thread may wake up
      // when the condition variable is signalled.
      pthread_res = pthread_mutex_unlock(&ctx->conn->incoming_lock);
      if (pthread_res != 0)
        adptc_support_todo;

      // Wake up the receiver thread
      // (if it is waiting on the condition variable).
      pthread_res = pthread_cond_signal(&ctx->conn->recv_has_work);
      if (pthread_res != 0)
        adptc_support_todo;

      // While the receiver thread is processing the incoming data,
      // begin the next read.
      state = state_read;
      break;

    default: adptc_support_todo;
    }
  }

  return NULL;
}

static void *recv_routine(void *const thread_arg) {
  struct recv_ctx *const ctx = thread_arg;
  assert(ctx);
  assert(ctx->conn);
  assert(ctx->mon);

  int pthread_res;

  pthread_res = pthread_mutex_lock(&ctx->conn->incoming_lock);
  if (pthread_res != 0)
    adptc_support_todo;

  struct monitor_ctx mon_ctx = {.conn = ctx->conn,
                                .continue_flag = &ctx->continue_flag};
  ctx->mon(&mon_ctx, ctx->mon_user_ctx);

  pthread_res = pthread_mutex_unlock(&ctx->conn->incoming_lock);
  if (pthread_res != 0)
    adptc_support_todo;

  return NULL;
}

void adptc_listen_start(adptc_listen const lhnd, int const serial_fd,
                        adptc_listen_monitor const mon,
                        void *const mon_user_ctx) {
  struct listen_ctx *const ctx = lhnd;
  assert(ctx);

  int pthread_res;

  atomic_store(&ctx->conn.incoming_out_for_delivery, false);
  atomic_flag_test_and_set(&ctx->conn.busy_flag);
  atomic_flag_test_and_set(&ctx->send_ctx.continue_flag);
  atomic_flag_test_and_set(&ctx->recv_ctx.continue_flag);

  pthread_mutexattr_t lock_attr;

  pthread_res = pthread_mutexattr_init(&lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  // TODO: check error
  pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_ERRORCHECK);

  pthread_res = pthread_mutex_init(&ctx->conn.incoming_lock, &lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_mutexattr_destroy(&lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_cond_init(&ctx->conn.recv_has_work, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  ctx->send_ctx.serial_fd = serial_fd;
  pthread_res =
      pthread_create(&ctx->send_thread, NULL, send_routine, &ctx->send_ctx);
  if (pthread_res != 0)
    adptc_support_todo;

  ctx->recv_ctx.mon = mon;
  ctx->recv_ctx.mon_user_ctx = mon_user_ctx;
  pthread_res =
      pthread_create(&ctx->recv_thread, NULL, recv_routine, &ctx->recv_ctx);
  if (pthread_res != 0)
    adptc_support_todo;
}

void adptc_listen_stop(adptc_listen const lhnd) {
  struct listen_ctx *const ctx = lhnd;
  assert(ctx);

  int pthread_res;

  // Indicate to the receiver thread that we are done.
  atomic_flag_clear(&ctx->recv_ctx.continue_flag);

  // If the receiver thread is waiting on the condition variable,
  // wake it up so that it can respond to the request to stop.
  pthread_res = pthread_cond_signal(&ctx->conn.recv_has_work);
  if (pthread_res != 0)
    adptc_support_todo;

  // Block until the receiver thread exits.
  pthread_res = pthread_join(ctx->recv_thread, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  // Indicate to the sender thread that we are done.
  atomic_flag_clear(&ctx->send_ctx.continue_flag);

  // Block until the sender thread exits.
  pthread_res = pthread_join(ctx->send_thread, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  // Destroy the condition variable.
  pthread_res = pthread_cond_destroy(&ctx->conn.recv_has_work);
  if (pthread_res != 0)
    adptc_support_todo;

  // Destroy the lock.
  pthread_res = pthread_mutex_destroy(&ctx->conn.incoming_lock);
  if (pthread_res != 0)
    adptc_support_todo;
}

adptc_listen_incoming
adptc_listen_accept_incoming(adptc_listen_sender const shnd) {
  struct monitor_ctx *const ctx = shnd;
  assert(ctx);
  assert(ctx->conn);

  int pthread_res;

  // Tell the sender thread
  // that we are done using the previous incoming data.
  atomic_flag_clear(&ctx->conn->busy_flag);

  bool should_continue;
  while ((should_continue = atomic_flag_test_and_set(ctx->continue_flag)) &&
         !atomic_load(&ctx->conn->incoming_out_for_delivery)) {
    // Wait until someone tells us to wake up.
    // NOTE: waiting on a condition variable releases the lock,
    //       allowing the sender thread to acquire it
    //       and set the 'out-for-delivery' flag.
    pthread_res =
        pthread_cond_wait(&ctx->conn->recv_has_work, &ctx->conn->incoming_lock);
    if (pthread_res != 0)
      adptc_support_todo;
  }

  // The main thread has requested for us to exit.
  // Return `NULL` so that the caller knows we are done.
  if (!should_continue)
    return NULL;

  // Tell the sender thread that we received the delivery.
  atomic_store(&ctx->conn->incoming_out_for_delivery, false);

  return &ctx->conn->incoming;
}

unsigned char const *
adptc_listen_get_incoming_data(adptc_listen_incoming const ihnd) {
  struct incoming *const incom = ihnd;
  assert(incom);

  return incom->data_buf;
}

size_t adptc_listen_get_incoming_data_len(adptc_listen_incoming const ihnd) {
  struct incoming *const incom = ihnd;
  assert(incom);

  return incom->data_len;
}

double adptc_listen_get_incoming_fill_ratio(adptc_listen_incoming const ihnd) {
  struct incoming *const incom = ihnd;
  assert(incom);

  return (double)incom->data_len / (double)LISTEN_BUF_SIZE;
}
