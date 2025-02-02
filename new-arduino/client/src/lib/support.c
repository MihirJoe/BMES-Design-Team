/// \file

#include <adapt/client/support.h>

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/// \internal
/// \brief The default panic handler.
///        Panics with a formatted message.
///
/// The message is formatted with vfprintf.
///`
/// In addition to the message,
/// the file name (__FILE__),
/// line number (__LINE__),
/// and function name (__func__)
/// at the call site are printed.
/// Output is written to stderr.
///
/// Call site information is expected to have been preserved
/// by the macro invoking adptc_support_panic_internal.
///
/// \param[in] file_name The file name at the call site.
/// \param[in] line_num The line number at the call site.
/// \param[in] func_name The function name at the call site.
/// \param[in] msg_fmt The format string of the message.
static void default_panic_handler(char const *file_name, long line_num,
                                  char const *func_name, char const *msg_fmt,
                                  va_list msg_ap,
                                  [[maybe_unused]] void *const hndlr_env) {
  fprintf(stderr, "\n\npanic in %s() at %s:%ld:\n  ", func_name, file_name,
          line_num);
  vfprintf(stderr, msg_fmt, msg_ap);
  fprintf(stderr, "\n\n");
}

/// \internal
static adptc_support_panic_handler panic_handler = default_panic_handler;
/// \internal
static void *panic_handler_env = NULL;

/// \internal
static void check_panic_handler(adptc_support_panic_handler const hndlr) {
  assert(hndlr);
}

void adptc_support_register_panic_handler(
    adptc_support_panic_handler const hndlr, void *const hndlr_env) {
  check_panic_handler(hndlr);

  panic_handler = hndlr;
  panic_handler_env = hndlr_env;
}

#ifndef NDEBUG
void adptc_support_debug_internal(char const *const file_name,
                                  long const line_num,
                                  char const *const msg_fmt, ...) {
  va_list ap;
  va_start(ap, msg_fmt);

  fprintf(stderr, "%s:%ld: ", file_name, line_num);
  vfprintf(stderr, msg_fmt, ap);
  fprintf(stderr, "\n");

  va_end(ap);
}
#endif

[[noreturn]] void adptc_support_panic_internal(char const *const file_name,
                                               long const line_num,
                                               char const *const func_name,
                                               char const *const msg_fmt, ...) {
  va_list msg_ap;
  va_start(msg_ap, msg_fmt);

  panic_handler(file_name, line_num, func_name, msg_fmt, msg_ap,
                panic_handler_env);

  va_end(msg_ap);

  abort();
}
