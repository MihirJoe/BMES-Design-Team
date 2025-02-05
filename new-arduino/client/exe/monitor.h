/// \file

#ifndef ADAPT_CLIENT_MONITOR_H
#define ADAPT_CLIENT_MONITOR_H

#include <adapt/client/listener.h>

#include <stdio.h>

/// \class adptc_monitor
typedef void *adptc_monitor;

/// \public \static \memberof adptc_monitor
adptc_monitor adptc_monitor_create(FILE *file);

/// \public \memberof adptc_monitor
void adptc_monitor_destroy(adptc_monitor monitor);

/// \public \static \memberof adptc_monitor
void adptc_monitor_callback(adptc_listener_incoming incoming, void *user_ctx);

#endif // ADAPT_CLIENT_MONITOR_H
