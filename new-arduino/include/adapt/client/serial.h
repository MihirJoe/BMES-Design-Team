/// \file
/// \brief Facilities for configuring a TTY as an ADAPT serial interface.

#ifndef ADAPT_CLIENT_SERIAL_H
#define ADAPT_CLIENT_SERIAL_H

#include <adapt/client/result.h>

// clang-format off
ADPTC_RESULT_WITH_OS_ERROR(adptc_serial_configure_result, adptc_serial_configure_result_k_tcgetattr_error, adptc_serial_configure_result_k_tcsetattr_error, adptc_serial_configure_result_k_cfsetispeed_error, adptc_serial_configure_result_k_cfsetospeed_error)
// clang-format on

/// \brief Attempts to configure a TTY as an ADAPT serial interface.
///
/// The TTY to be configured is identified by the file descriptor \a fd.
///
/// \param[in] fd The TTY file descriptor.
adptc_serial_configure_result adptc_serial_try_configure(int fd);

#endif // ADAPT_CLIENT_SERIAL_H
