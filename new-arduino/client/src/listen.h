#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <pthread.h>

#include <stdatomic.h>
#include <stdbool.h>

typedef void *adptc_listen_handle;
typedef void *adptc_listen_sender_handle;
typedef void *adptc_listen_incoming_handle;

typedef void (*adptc_listen_monitor)(adptc_listen_sender_handle sender,
                                     void *user_ctx);

adptc_listen_handle adptc_listen_create(void);
void adptc_listen_destroy(adptc_listen_handle listen);

void adptc_listen_start(adptc_listen_handle listen, int serial_fd,
                        adptc_listen_monitor mon, void *mon_user_ctx);
void adptc_listen_stop(adptc_listen_handle listen);

adptc_listen_incoming_handle
adptc_listen_accept_incoming(adptc_listen_sender_handle sender);
unsigned char const *
adptc_listen_get_incoming_data(adptc_listen_incoming_handle incoming);
size_t
adptc_listen_get_incoming_data_len(adptc_listen_incoming_handle incoming);

#endif // ADAPT_CLIENT_LISTEN_H
