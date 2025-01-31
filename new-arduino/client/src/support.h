#ifndef ADAPT_CLIENT_SUPPORT_H
#define ADAPT_CLIENT_SUPPORT_H

#define adptc_support_panic_str(str)                                           \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (str))
#define adptc_support_panic_fmt(fmt, ...)                                      \
  adptc_support_panic_internal(__FILE__, __LINE__, __func__, (fmt), __VA_ARGS__)

#define adptc_support_todo adptc_support_panic_str("not implemented")

_Noreturn void adptc_support_panic_internal(char const *file_name,
                                            long line_num,
                                            char const *func_name,
                                            char const *fmt, ...);

#endif // ADAPT_CLIENT_SUPPORT_H
