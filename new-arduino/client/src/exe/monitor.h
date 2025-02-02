#ifndef ADAPT_CLIENT_MONITOR_H
#define ADAPT_CLIENT_MONITOR_H

#include <adapt/client/listener.h>

#include <stdatomic.h>
#include <stdio.h>

struct adptc_monitor_ctx {
  FILE *file;
};

void adptc_monitor(adptc_listener_receiver sender, void *user_ctx);

#endif // ADAPT_CLIENT_MONITOR_H
