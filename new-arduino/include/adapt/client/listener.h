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
/// when incoming data is available.
typedef void (*adptc_listener_callback)(adptc_listener_incoming incoming,
                                        void *callback_env);

/// \cond
ADPTC_RESULT_WITH_OK_TYPE_AND_OS_ERROR(
    adptc_listener_create_result, adptc_listener,
    adptc_listener_create_result_k_alloc_listener_error,
    adptc_listener_create_result_k_mutexattr_init_error,
    adptc_listener_create_result_k_mutexattr_settype_error,
    adptc_listener_create_result_k_mutex_init_error,
    adptc_listener_create_result_k_cond_init_error)

ADPTC_RESULT_WITH_OS_ERROR(
    adptc_listener_start_result,
    adptc_listener_start_result_k_create_sender_thread_error,
    adptc_listener_start_result_k_create_receiver_thread_error)
/// \endcond

/// \public \static \memberof adptc_listener
/// \brief
/// Attempts to create a new listener.
///
/// \return
/// A new listener.
[[nodiscard]] adptc_listener_create_result adptc_listener_try_create(void);

/// \public \memberof adptc_listener
/// \brief
/// Destroys a listener.
///
/// This method releases resources owned by \a listener.
///
/// On return from this method,
/// \a listener no longer exists
/// and must not be used in further method calls.
void adptc_listener_destroy(adptc_listener listener);

/// \public \memberof adptc_listener
/// \brief
/// Starts listening for ADAPT serial data.
///
/// \a callback is called when incoming data is available.
///
/// \note
/// \a callback is called from a different thread
/// than the caller of this method.
///
/// \param[in] serial_fd
/// The open file descriptor
/// configured as an ADAPT serial interface.
/// \param[in] callback
/// A pointer to the function to be called
/// when incoming data is available.
/// \param[in] callback_env
/// An arbitrary pointer passed to \a callback.
adptc_listener_start_result
adptc_listener_try_start(adptc_listener listener, int serial_fd,
                         adptc_listener_callback callback, void *callback_env);

/// \public \memberof adptc_listener
/// \brief
/// Stops listening for ADAPT serial data.
///
/// \pre
/// \a listener is listening.
void adptc_listener_stop(adptc_listener listener);

/// \public \static \memberof adptc_listener_incoming
/// \brief
/// The upper bound on the number of incoming bytes.
///
/// \ref adptc_listener_incoming_get_len will return a value
/// no greater than this.
extern size_t const adptc_listener_incoming_buf_size;

/// \public \memberof adptc_listener_incoming
/// \brief
/// Gets a pointer to the array of incoming bytes.
///
/// The length of this array
/// is given by \ref adptc_listener_incoming_get_len.
///
/// \return
/// A pointer to the array of incoming bytes.
[[nodiscard]] unsigned char const *
adptc_listener_incoming_get_ptr(adptc_listener_incoming incoming);

/// \public \memberof adptc_listener_incoming
/// \brief
/// Gets the number of incoming bytes.
///
/// \return
/// The number of incoming bytes.
[[nodiscard]] size_t
adptc_listener_incoming_get_len(adptc_listener_incoming incoming);

#endif // ADAPT_CLIENT_LISTEN_H
