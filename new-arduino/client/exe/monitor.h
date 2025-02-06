/// \file
/// Write incoming ADAPT serial messages to a file.

#ifndef ADAPT_CLIENT_MONITOR_H
#define ADAPT_CLIENT_MONITOR_H

#include <adapt/client/listener.h>
#include <adapt/client/result.h>

#include <stdio.h>

/// \class adptc_monitor
/// \brief
/// Writes incoming ADAPT serial messages to a file.
typedef void *adptc_monitor;

/// \public \static \memberof adptc_monitor
/// \brief
/// Attempts to create a new monitor.
///
/// \param[in] file
/// The file to which incoming ADAPT serial messages are written.
adptc_monitor adptc_monitor_try_create(FILE *file);

/// \public \memberof adptc_monitor
/// \brief
/// Destroys a monitor.
///
/// This method releases resources owned by \a monitor.
///
/// \note
/// This method does not close the file.
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
