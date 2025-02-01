#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <pthread.h>

#include <stdatomic.h>
#include <stdbool.h>

#define ADPTC_LISTEN_BUF_SIZE 256

#define ADPTC_LISTEN_SYS_INIT                                                  \
  ((struct adptc_listen_sys){.conn = {.busy_flag = ATOMIC_FLAG_INIT},          \
                             .send_ctx = {.continue_flag = ATOMIC_FLAG_INIT},  \
                             .recv_ctx = {.continue_flag = ATOMIC_FLAG_INIT}})

typedef void *adptc_listen_handle;
typedef void (*adptc_listen_monitor)(adptc_listen_handle lhnd, void *user_ctx);

struct adptc_listen_incoming {
  unsigned char data_buf[ADPTC_LISTEN_BUF_SIZE];
  size_t data_len;
};

struct adptc_listen_conn {
  struct adptc_listen_incoming incoming;
  unsigned char read_buf[ADPTC_LISTEN_BUF_SIZE];
  atomic_bool incoming_out_for_delivery;
  atomic_flag busy_flag;
  pthread_mutex_t incoming_lock;
  pthread_cond_t mon_has_work;
};

struct adptc_listen_send_ctx {
  struct adptc_listen_conn *conn;
  int serial_fd;
  atomic_flag continue_flag;
};

struct adptc_listen_recv_ctx {
  struct adptc_listen_conn *conn;
  adptc_listen_monitor mon;
  void *mon_user_ctx;
  atomic_flag continue_flag;
};

struct adptc_listen_sys {
  struct adptc_listen_conn conn;
  struct adptc_listen_send_ctx send_ctx;
  struct adptc_listen_recv_ctx recv_ctx;
  pthread_t send_thread;
  pthread_t recv_thread;
};

void adptc_listen_start(struct adptc_listen_sys *lsys, int serial_fd,
                        adptc_listen_monitor mon, void *mon_user_ctx);
void adptc_listen_stop(struct adptc_listen_sys *lsys);

struct adptc_listen_incoming const *
adptc_listen_accept_incoming(adptc_listen_handle lhnd);

#endif // ADAPT_CLIENT_LISTEN_H
