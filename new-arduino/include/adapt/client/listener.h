#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <stddef.h>

typedef void *adptc_listener;
typedef void *adptc_listener_receiver;
typedef void *adptc_listener_incoming;

typedef void (*adptc_listener_routine)(adptc_listener_receiver receiver,
                                       void *user_ctx);

[[nodiscard]] adptc_listener adptc_listener_create(void);
void adptc_listener_destroy(adptc_listener listener);

void adptc_listener_start(adptc_listener listener, int serial_fd,
                          adptc_listener_routine routine, void *user_ctx);
void adptc_listener_stop(adptc_listener listener);

adptc_listener_incoming
adptc_listener_accept_incoming(adptc_listener_receiver receiver);

[[nodiscard]] unsigned char const *
adptc_listener_get_incoming_data(adptc_listener_incoming incoming);
[[nodiscard]] size_t
adptc_listener_get_incoming_data_len(adptc_listener_incoming incoming);
[[nodiscard]] double
adptc_listener_get_incoming_fill_ratio(adptc_listener_incoming incoming);

#endif // ADAPT_CLIENT_LISTEN_H
