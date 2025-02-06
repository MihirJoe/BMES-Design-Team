/// \file

#ifndef ADAPT_CLIENT_CONSOLE_H
#define ADAPT_CLIENT_CONSOLE_H

#include <adapt/client/result.h>

/// \cond
ADPTC_RESULT(adptc_console_attend_result,
             adptc_console_attend_result_k_stdin_error)
/// \endcond

adptc_console_attend_result adptc_console_try_attend(int serial_fd);

#endif // ADAPT_CLIENT_CONSOLE_H
