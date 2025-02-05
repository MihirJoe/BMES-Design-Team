/// \file
/// \brief
/// Facilities for configuring an open file descriptor
/// as an ADAPT serial interface.

#ifndef ADAPT_CLIENT_SERIAL_H
#define ADAPT_CLIENT_SERIAL_H

#include <adapt/client/result.h>

/// \cond
ADPTC_RESULT_WITH_OS_ERROR(adptc_serial_configure_result,
                           adptc_serial_configure_result_k_tcgetattr_error,
                           adptc_serial_configure_result_k_tcsetattr_error,
                           adptc_serial_configure_result_k_cfsetispeed_error,
                           adptc_serial_configure_result_k_cfsetospeed_error)
/// \endcond

/// \brief
/// Attempts to configure an open file descriptor
/// as an ADAPT serial interface.
///
/// On Linux, the path to the ADAPT serial interface
/// looks like <tt>/dev/ttyACM<i>n</i></tt>
/// where \e n is a non-negative integer.
/// The 'ACM' in <tt>ttyACM</tt>
/// refers to a USB Communication Device Class (CDC) device
/// of the Abstract Control Model subclass.
/// CDC ACM is a generic protocol for raw data transfer.
///
/// On failure, the open file descriptor is unmodified.
/// In other words,
/// a partial configuration is not applied.
///
/// \param[in] fd
/// The open file descriptor to configure.
/// \return
/// The result indicating the success or failure of the operation.
adptc_serial_configure_result adptc_serial_try_configure(int fd);

#endif // ADAPT_CLIENT_SERIAL_H
