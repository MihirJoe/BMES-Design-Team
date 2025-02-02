/// \file

#ifndef ADAPT_CLIENT_CONSOLE_H
#define ADAPT_CLIENT_CONSOLE_H

#include <adapt/client/result.h>

// clang-format off
ADPTC_RESULT(adptc_console_attend_result, adptc_console_attend_result_k_stdin_error)
// clang-format on

adptc_console_attend_result adptc_console_try_attend(int serial_fd);

#endif // ADAPT_CLIENT_CONSOLE_H
