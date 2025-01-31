#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <pthread.h>

#include <stdatomic.h>

#define ADPTC_LISTEN_BUF_SIZE 256

#define ADPTC_LISTEN_CONN_INIT                                                 \
  ((struct adptc_listen_conn){.busy_flag = ATOMIC_FLAG_INIT})

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
  pthread_cond_t incoming_avail;
};

struct adptc_listen_ctx {
  atomic_flag *continue_flag;
  struct adptc_listen_conn *conn;
  int serial_fd;
};

void adptc_listen_open_conn(struct adptc_listen_conn *conn);
void adptc_listen_close_conn(struct adptc_listen_conn *conn);
void adptc_listen_register_receiver(struct adptc_listen_conn *conn);
void adptc_listen_unregister_receiver(struct adptc_listen_conn *conn);
void adptc_listen_wait_for_incoming(struct adptc_listen_conn *conn);
struct adptc_listen_incoming const *
adptc_listen_accept_incoming(struct adptc_listen_conn *conn);
void adptc_listen_end_inspection(struct adptc_listen_conn *conn);
void *adptc_listen_main(void *thread_arg);

#endif // ADAPT_CLIENT_LISTEN_H
