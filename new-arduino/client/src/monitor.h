#ifndef ADAPT_CLIENT_MONITOR_H
#define ADAPT_CLIENT_MONITOR_H

#include "listen.h"

#include <stdatomic.h>
#include <stdio.h>

struct adptc_monitor_ctx {
  atomic_flag *continue_flag;
  struct adptc_listen_conn *conn;
  FILE *file;
};

void *adptc_monitor_main(void *thread_arg);

#endif // ADAPT_CLIENT_MONITOR_H
