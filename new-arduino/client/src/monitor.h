#ifndef ADAPT_CLIENT_MONITOR_H
#define ADAPT_CLIENT_MONITOR_H

#include "listen.h"

#include <stdatomic.h>
#include <stdio.h>

struct adptc_monitor_ctx {
  FILE *file;
};

void adptc_monitor(adptc_listen_handle lhnd, void *user_ctx);

#endif // ADAPT_CLIENT_MONITOR_H
