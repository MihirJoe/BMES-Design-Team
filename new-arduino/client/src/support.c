#include "support.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

_Noreturn void adptc_support_panic_internal(char const *file_name,
                                            long line_num,
                                            char const *func_name,
                                            char const *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "panic in %s() at %s:%ld:\n", func_name, file_name, line_num);
  fprintf(stderr, "  ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");

  va_end(ap);

  abort();
}
