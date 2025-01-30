#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <stdatomic.h>

struct adptc_listen_ctx {
  int serial_fd;
  atomic_flag *continue_flag;
};

void *adptc_listen_main(void *thread_arg);

#endif // ADAPT_CLIENT_LISTEN_H
