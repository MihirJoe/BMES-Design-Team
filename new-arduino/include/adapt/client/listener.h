/// \file
/// \brief
/// Asynchronous notification of incoming ADAPT serial data.

#ifndef ADAPT_CLIENT_LISTEN_H
#define ADAPT_CLIENT_LISTEN_H

#include <adapt/client/result.h>

#include <stddef.h>

/// \class adptc_listener
/// \brief
/// Listens for ADAPT serial data.
typedef void *adptc_listener;

/// \class adptc_listener_incoming
/// \brief
/// Incoming ADAPT serial data.
typedef void *adptc_listener_incoming;

/// \brief
/// A pointer to a function to be called
/// when ADAPT serial data is received.
typedef void (*adptc_listener_callback)(adptc_listener_incoming incoming,
                                        void *callback_env);

/// \public \static \memberof adptc_listener
[[nodiscard]] adptc_listener adptc_listener_create(void);

/// \public \memberof adptc_listener
void adptc_listener_destroy(adptc_listener listener);

/// \public \memberof adptc_listener
void adptc_listener_start(adptc_listener listener, int serial_fd,
                          adptc_listener_callback callback, void *callback_env);

/// \public \memberof adptc_listener
void adptc_listener_stop(adptc_listener listener);

/// \public \static \memberof adptc_listener_incoming
extern size_t const adptc_listener_incoming_buf_size;

/// \public \memberof adptc_listener_incoming
[[nodiscard]] unsigned char const *
adptc_listener_incoming_get_ptr(adptc_listener_incoming incoming);

/// \public \memberof adptc_listener_incoming
[[nodiscard]] size_t
adptc_listener_incoming_get_len(adptc_listener_incoming incoming);

#endif // ADAPT_CLIENT_LISTEN_H
