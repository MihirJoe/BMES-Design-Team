#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <pthread.h>

#include <stdatomic.h>
#include <stdbool.h>

typedef void *adptc_listen;
typedef void *adptc_listen_sender;
typedef void *adptc_listen_incoming;

typedef void (*adptc_listen_monitor)(adptc_listen_sender sender,
                                     void *user_ctx);

adptc_listen adptc_listen_create(void);
void adptc_listen_destroy(adptc_listen listen);

void adptc_listen_start(adptc_listen listen, int serial_fd,
                        adptc_listen_monitor mon, void *mon_user_ctx);
void adptc_listen_stop(adptc_listen listen);

adptc_listen_incoming adptc_listen_accept_incoming(adptc_listen_sender sender);

unsigned char const *
adptc_listen_get_incoming_data(adptc_listen_incoming incoming);
size_t adptc_listen_get_incoming_data_len(adptc_listen_incoming incoming);
double adptc_listen_get_incoming_fill_ratio(adptc_listen_incoming incoming);

#endif // ADAPT_CLIENT_LISTEN_H
