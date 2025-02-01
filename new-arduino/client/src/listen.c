#include "listen.h"

#include "support.h"

#include <adapt/proto.h>

#include <pthread.h>
#include <unistd.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

struct adptc_listen_monitor_ctx {
  struct adptc_listen_conn *conn;
  atomic_flag *continue_flag;
};

static void *send_routine(void *const thread_arg) {
  struct adptc_listen_send_ctx *const ctx = thread_arg;
  assert(ctx);
  assert(ctx->conn);

  enum state {
    state_read,
    state_wait,
    state_deliver,
  };

  int pthread_res;

  enum state state = state_read;
  ssize_t read_res;

  while (atomic_flag_test_and_set(&ctx->continue_flag)) {
    switch (state) {
    case state_read:
      read_res =
          read(ctx->serial_fd, ctx->conn->read_buf, ADPTC_LISTEN_BUF_SIZE);
      if (read_res <= 0)
        // TODO
        read_res = 0;

      state = state_wait;
      break;

    case state_wait:
      if (!atomic_flag_test_and_set(&ctx->conn->busy_flag))
        state = state_deliver;

      break;

    case state_deliver:;
      size_t const read_len = read_res;
      memcpy(ctx->conn->incoming.data_buf, ctx->conn->read_buf, read_len);
      ctx->conn->incoming.data_len = read_len;

      pthread_res = pthread_mutex_lock(&ctx->conn->incoming_lock);
      if (pthread_res != 0)
        adptc_support_todo;

      atomic_store(&ctx->conn->incoming_out_for_delivery, true);

      pthread_res = pthread_mutex_unlock(&ctx->conn->incoming_lock);
      if (pthread_res != 0)
        adptc_support_todo;

      pthread_res = pthread_cond_signal(&ctx->conn->mon_has_work);
      if (pthread_res != 0)
        adptc_support_todo;

      state = state_read;
      break;

    default: adptc_support_todo;
    }
  }

  return NULL;
}

static void *recv_routine(void *const thread_arg) {
  struct adptc_listen_recv_ctx *const ctx = thread_arg;
  assert(ctx);
  assert(ctx->conn);
  assert(ctx->mon);

  int pthread_res;

  pthread_res = pthread_mutex_lock(&ctx->conn->incoming_lock);
  if (pthread_res != 0)
    adptc_support_todo;

  struct adptc_listen_monitor_ctx mon_ctx = {
      .conn = ctx->conn, .continue_flag = &ctx->continue_flag};
  ctx->mon(&mon_ctx, ctx->mon_user_ctx);

  pthread_res = pthread_mutex_unlock(&ctx->conn->incoming_lock);
  if (pthread_res != 0)
    adptc_support_todo;

  return NULL;
}

void adptc_listen_start(struct adptc_listen_sys *const ls, int const serial_fd,
                        adptc_listen_monitor const mon,
                        void *const mon_user_ctx) {
  int pthread_res;

  atomic_store(&ls->conn.incoming_out_for_delivery, false);
  // TODO: don't set busy flag
  atomic_flag_test_and_set(&ls->conn.busy_flag);
  atomic_flag_test_and_set(&ls->send_ctx.continue_flag);
  atomic_flag_test_and_set(&ls->recv_ctx.continue_flag);

  pthread_mutexattr_t lock_attr;
  // TODO: check error
  pthread_mutexattr_init(&lock_attr);
  pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_ERRORCHECK);

  pthread_res = pthread_mutex_init(&ls->conn.incoming_lock, &lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  // TODO: check error
  pthread_mutexattr_destroy(&lock_attr);

  pthread_res = pthread_cond_init(&ls->conn.mon_has_work, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  ls->send_ctx.conn = &ls->conn;
  ls->send_ctx.serial_fd = serial_fd;
  pthread_res =
      pthread_create(&ls->send_thread, NULL, send_routine, &ls->send_ctx);
  if (pthread_res != 0)
    adptc_support_todo;

  ls->recv_ctx.conn = &ls->conn;
  ls->recv_ctx.mon = mon;
  ls->recv_ctx.mon_user_ctx = mon_user_ctx;
  pthread_res =
      pthread_create(&ls->recv_thread, NULL, recv_routine, &ls->recv_ctx);
  if (pthread_res != 0)
    adptc_support_todo;
}

void adptc_listen_stop(struct adptc_listen_sys *const ls) {
  assert(ls);

  int pthread_res;

  atomic_flag_clear(&ls->recv_ctx.continue_flag);

  pthread_res = pthread_cond_signal(&ls->conn.mon_has_work);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_join(ls->recv_thread, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  atomic_flag_clear(&ls->send_ctx.continue_flag);

  pthread_res = pthread_join(ls->send_thread, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_cond_destroy(&ls->conn.mon_has_work);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_mutex_destroy(&ls->conn.incoming_lock);
  if (pthread_res != 0)
    adptc_support_todo;
}

struct adptc_listen_incoming const *
adptc_listen_accept_incoming(adptc_listen_handle const lhnd) {
  struct adptc_listen_monitor_ctx *const ctx = lhnd;
  assert(ctx);

  int pthread_res;

  atomic_flag_clear(&ctx->conn->busy_flag);

  bool should_continue;
  while ((should_continue = atomic_flag_test_and_set(ctx->continue_flag)) &&
         !atomic_load(&ctx->conn->incoming_out_for_delivery)) {
    pthread_res =
        pthread_cond_wait(&ctx->conn->mon_has_work, &ctx->conn->incoming_lock);
    if (pthread_res != 0)
      adptc_support_todo;
  }

  if (!should_continue)
    return NULL;

  atomic_store(&ctx->conn->incoming_out_for_delivery, false);

  return &ctx->conn->incoming;
}
