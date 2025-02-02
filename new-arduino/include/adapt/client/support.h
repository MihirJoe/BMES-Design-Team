/// \file
/// \brief Miscellaneous supporting facilities.

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

/// \brief Panics with a message.
///
/// The panic handler is invoked with the call site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to stderr the formatted message with vfprintf
/// and then calls abort.
///
/// \param[in] msg The message.
///
/// Control does not return to the caller after invocation of this macro.
#define adptc_support_panic_str(msg)                                           \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (msg))

/// \brief Panics with a formatted message.
///
/// The panic handler is invoked with the call site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to stderr the formatted message with vfprintf
/// and then calls abort.
///
/// \param[in] msg_fmt The format string of the message.
///
/// Control does not return to the caller after invocation of this macro.
#define adptc_support_panic_fmt(msg_fmt, ...)                                  \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (msg_fmt),        \
                               __VA_ARGS__)

/// \brief Panics with a preset message
///        indicating that the desired functionality is not implemented.
///
/// The panic handler is invoked with the call site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to stderr the formatted message with vfprintf
/// and then calls abort.
///
/// Control does not return to the caller after invocation of this macro.
#define adptc_support_todo adptc_support_panic_str("not implemented")

typedef void (*adptc_support_panic_handler)(char const *file_name,
                                            long line_num,
                                            char const *func_name,
                                            char const *msg_fmt, va_list msg_ap,
                                            void *handler_env);

/// \brief Registers a panic handler.
///
/// \warning This function is not thread-safe.
void adptc_support_register_panic_handler(adptc_support_panic_handler handler,
                                          void *handler_env);

#ifndef NDEBUG
/// \internal
/// \brief Emits a formatted message for debugging.
///
/// \note This function is not defined when NDEBUG is defined.
///
/// \note This function is intended to be invoked by a macro
///       that preserves call site information.
///
/// The message is formatted with vfprintf.
///
/// In addition to the message,
/// the file name (__FILE__)
/// and line number (__LINE__)
/// at the call site are printed.
/// Output is written to stderr.
///
/// \param[in] file_name The file name at the call site.
/// \param[in] line_num The line number at the call site.
/// \param[in] msg_fmt The format string of the message.
void adptc_support_debug_internal(char const *file_name, long line_num,
                                  char const *msg_fmt, ...);
#endif

/// \internal
/// \brief Panics with a formatted message.
///
/// \note This function is intended to be invoked by a macro
///       that preserves call site information.
///
/// The panic handler is invoked with the call site information and message.
/// If a panic handler was not registered,
/// a default handler is invoked
/// that prints to \c stderr the formatted message with \c vfprintf
/// and then calls \c abort.
///
/// \param[in] file_name The file name at the call site.
/// \param[in] line_num The line number at the call site.
/// \param[in] func_name The function name at the call site.
/// \param[in] msg_fmt The format string of the message.
///
/// This function does not return.
[[noreturn]] void adptc_support_panic_internal(char const *file_name,
                                               long line_num,
                                               char const *func_name,
                                               char const *msg_fmt, ...);

#endif // ADAPT_CLIENT_SUPPORT_H
