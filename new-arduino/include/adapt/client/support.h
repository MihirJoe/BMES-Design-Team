/// \file
/// \brief
/// Miscellaneous supporting facilities.

#ifndef ADAPT_CLIENT_SUPPORT_H
#define ADAPT_CLIENT_SUPPORT_H

#include <stdarg.h>

#ifdef NDEBUG
#define adptc_support_debug_str(msg) ((void)0)
#define adptc_support_debug_fmt(msg_fmt, ...) ((void)0)
#else
#define adptc_support_debug_str(msg)                                           \
  adptc_support_debug_internal(__FILE__, __LINE__, (msg))
#define adptc_support_debug_fmt(msg_fmt, ...)                                  \
  adptc_support_debug_internal(__FILE__, __LINE__, (msg_fmt), __VA_ARGS__)
#endif

/// \brief
/// Panics with a message.
///
/// The panic handler is invoked with the panic site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to \c stderr the formatted message with \c vfprintf.
///
/// The process aborts once the panic handler returns.
///
/// \param[in] msg
/// The message string.
#define adptc_support_panic_str(msg)                                           \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (msg))

/// \brief
/// Panics with a formatted message.
///
/// The panic handler is invoked with the panic site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to \c stderr the formatted message with \c vfprintf.
///
/// The process aborts once the panic handler returns.
///
/// \param[in] msg_fmt
/// The message format string.
#define adptc_support_panic_fmt(msg_fmt, ...)                                  \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (msg_fmt),        \
                               __VA_ARGS__)

/// \brief
/// Panics with a preset message
/// indicating that the desired functionality is not implemented.
///
/// The panic handler is invoked with the panic site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to \c stderr the formatted message with \c vfprintf.
///
/// The process aborts once the panic handler returns.
#define adptc_support_todo adptc_support_panic_str("not implemented")

typedef void (*adptc_support_panic_handler)(char const *file_name,
                                            long line_num,
                                            char const *func_name,
                                            char const *msg_fmt, va_list msg_ap,
                                            void *handler_env);

/// \brief
/// Registers a panic handler.
///
/// \warning
/// This function is not thread-safe.
void adptc_support_register_panic_handler(adptc_support_panic_handler handler,
                                          void *handler_env);

#ifndef NDEBUG
/// \brief
/// Emits a formatted message for debugging.
///
/// \note
/// This function is not defined when \c NDEBUG is defined.
///
/// The message is formatted with \c vfprintf.
/// In addition to the message,
/// a file name and line number are printed.
/// All output is written to \c stderr.
///
/// \param[in] file_name
/// The file name.
/// \param[in] line_num
/// The line number.
/// \param[in] msg_fmt
/// The message format.
void adptc_support_debug_internal(char const *file_name, long line_num,
                                  char const *msg_fmt, ...);
#endif

/// \brief
/// Panics with a formatted message.
///
/// The panic handler is invoked with the panic site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to \c stderr the formatted message with \c vfprintf.
///
/// The process aborts once the panic handler returns.
///
/// \param[in] file_name
/// The file name at the panic site.
/// \param[in] line_num
/// The line number at the panic site.
/// \param[in] func_name
/// The function name at the panic site.
/// \param[in] msg_fmt
/// The message format.
///
/// \note
/// This function does not return.
[[noreturn]] void adptc_support_panic_internal(char const *file_name,
                                               long line_num,
                                               char const *func_name,
                                               char const *msg_fmt, ...);

#endif // ADAPT_CLIENT_SUPPORT_H
