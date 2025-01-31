#include "listen.h"

#include "support.h"

#include <adapt/proto.h>

#include <pthread.h>
#include <unistd.h>

#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// TODO: check for pthread errors

static struct timespec const accept_incoming_timeout = {.tv_sec = 0,
                                                        .tv_nsec = 1000000l};

void adptc_listen_open_conn(struct adptc_listen_conn *const conn) {
  assert(conn);

  atomic_store(&conn->incoming_out_for_delivery, false);
  // TODO: check error
  pthread_mutex_init(&conn->incoming_lock, NULL);
  pthread_cond_init(&conn->incoming_avail, NULL);
}

void adptc_listen_close_conn(struct adptc_listen_conn *const conn) {
  assert(conn);

  // TODO: check error
  pthread_cond_destroy(&conn->incoming_avail);
  pthread_mutex_destroy(&conn->incoming_lock);
}

void adptc_listen_register_receiver(struct adptc_listen_conn *const conn) {
  assert(conn);

  // TODO: check error
  pthread_mutex_lock(&conn->incoming_lock);
}

void adptc_listen_unregister_receiver(struct adptc_listen_conn *const conn) {
  assert(conn);

  // TODO: check error
  pthread_mutex_unlock(&conn->incoming_lock);
}

void adptc_listen_wait_for_incoming(struct adptc_listen_conn *const conn) {
  int const timedwait_res = pthread_cond_timedwait(
      &conn->incoming_avail, &conn->incoming_lock, &accept_incoming_timeout);
  if (timedwait_res != 0 && timedwait_res != ETIMEDOUT)
    adptc_support_todo;
}

struct adptc_listen_incoming const *
adptc_listen_accept_incoming(struct adptc_listen_conn *const conn) {
  assert(conn);

  if (!atomic_load(&conn->incoming_out_for_delivery))
    return NULL;

  atomic_store(&conn->incoming_out_for_delivery, false);

  return &conn->incoming;
}

void adptc_listen_end_inspection(struct adptc_listen_conn *const conn) {
  assert(conn);

  atomic_flag_clear(&conn->busy_flag);
}

void *adptc_listen_main(void *const thread_arg) {
  struct adptc_listen_ctx const *const ctx = thread_arg;
  assert(ctx);
  assert(ctx->conn);

  enum state {
    state_read,
    state_wait_to_fill_incoming,
    state_fill_incoming,
  };

  enum state state = state_read;
  ssize_t read_res;

  while (atomic_flag_test_and_set(ctx->continue_flag)) {
    switch (state) {
    case state_read:
      read_res =
          read(ctx->serial_fd, ctx->conn->read_buf, ADPTC_LISTEN_BUF_SIZE);
      if (read_res <= 0)
        // TODO
        read_res = 0;

      state = state_wait_to_fill_incoming;
      break;

    case state_wait_to_fill_incoming:
      if (!atomic_flag_test_and_set(&ctx->conn->busy_flag))
        state = state_fill_incoming;

      break;

    case state_fill_incoming:;
      size_t const read_len = read_res;
      memcpy(ctx->conn->incoming.data_buf, ctx->conn->read_buf, read_len);
      ctx->conn->incoming.data_len = read_len;

      // TODO: check error
      pthread_mutex_lock(&ctx->conn->incoming_lock);
      atomic_store(&ctx->conn->incoming_out_for_delivery, true);

      int const signal_res = pthread_cond_signal(&ctx->conn->incoming_avail);
      if (signal_res != 0)
        adptc_support_todo;

      state = state_read;
      break;

    default: adptc_support_todo;
    }
  }

  return NULL;
}
