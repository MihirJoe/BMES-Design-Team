/// \file
/// Write incoming ADAPT serial messages to a file.

#ifndef ADAPT_CLIENT_MONITOR_H
#define ADAPT_CLIENT_MONITOR_H

#include <adapt/client/listener.h>
#include <adapt/client/proto.h>
#include <adapt/client/result.h>

#include <stdio.h>

/// \class adptc_monitor
/// \brief
/// Writes incoming ADAPT serial messages to a file.
typedef void *adptc_monitor;

/// \cond
ADPTC_RESULT_WITH_OK_TYPE_AND_OS_ERROR(
    adptc_monitor_create_result, adptc_monitor,
    adptc_monitor_create_result_k_alloc_monitor_error)
/// \endcond

/// \public \static \memberof adptc_monitor
/// \brief
/// Attempts to create a new monitor.
///
/// On success,
/// ownership of \a response_decoder
/// is transferred to the new monitor.
/// \ref adptc_proto_response_decoder_destroy need not,
/// and in fact should not,
/// be called to destroy the decoder;
/// it will be destroyed by \ref adptc_monitor_destroy.
///
/// Ownership of \a file
/// is \e not transferred to the new monitor.
/// In other words,
/// \a file is not closed when the monitor is destroyed.
///
/// \param[in] file
/// The file to which incoming ADAPT serial messages are written.
/// \param[in] response_decoder
/// The response decoder.
adptc_monitor_create_result
adptc_monitor_try_create(FILE *file,
                         adptc_proto_response_decoder response_decoder);

/// \public \memberof adptc_monitor
/// \brief
/// Destroys a monitor.
///
/// Resources owned by \a monitor are released.
///
/// On return from this method,
/// \a monitor no longer exists
/// and must not be used in further method calls.
void adptc_monitor_destroy(adptc_monitor monitor);

/// \public \static \memberof adptc_monitor
/// \brief
/// The callback function that should passed to \ref adptc_listener_start.
void adptc_monitor_callback(adptc_listener_incoming incoming, void *user_ctx);

#endif // ADAPT_CLIENT_MONITOR_H
