#ifndef ADAPT_CLIENT_SUPPORT_H
#define ADAPT_CLIENT_SUPPORT_H

#ifdef NDEBUG
#define adptc_support_debug_str(str) ((void)0)
#define adptc_support_debug_fmt(str) ((void)0)
#else
#define adptc_support_debug_str(str)                                           \
  adptc_support_debug_internal(__FILE__, __LINE__, (str))
#define adptc_support_debug_fmt(fmt, ...)                                      \
  adptc_support_debug_internal(__FILE__, __LINE__, (fmt), __VA_ARGS__)
#endif

#define adptc_support_panic_str(str)                                           \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (str))
#define adptc_support_panic_fmt(fmt, ...)                                      \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (fmt), __VA_ARGS__)

#define adptc_support_todo adptc_support_panic_str("not implemented")

void adptc_support_debug_internal(char const *file_name, long line_num,
                                  char const *fmt, ...);
[[noreturn]] void adptc_support_panic_internal(char const *file_name,
                                               long line_num,
                                               char const *func_name,
                                               char const *fmt, ...);

#endif // ADAPT_CLIENT_SUPPORT_H
