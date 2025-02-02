/// \file

#ifndef ADAPT_CLIENT_OS_H
#define ADAPT_CLIENT_OS_H

#include <errno.h>
#include <stdio.h>

#define adptc_os_call(func, ...)                                               \
  ((struct adptc_os_error){                                                    \
      .func_name = #func, .code = ((func)(__VA_ARGS__)), errno})

struct adptc_os_error {
  char const *func_name;
  int code;
};

void adptc_os_print_error(FILE *out, struct adptc_os_error error);

#endif // ADAPT_CLIENT_RESULT_H
