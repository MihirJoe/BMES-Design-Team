#ifndef ADAPT_CLIENT_SERIAL_H
#define ADAPT_CLIENT_SERIAL_H

#include <stdatomic.h>

struct adapt_client_serial_context {
  int fd;
  atomic_flag *continue_flag;
};

void *adapt_client_serial_main(void *thread_arg);

#endif // ADAPT_CLIENT_SERIAL_H
