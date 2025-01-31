#include "result.h"

#include <assert.h>
#include <string.h>

#define RESULT_KIND_COUNT (sizeof(kind_to_str) / sizeof(kind_to_str[0]))

static char const *const kind_to_str[] = {
    [adptc_result_k_ok] = "OK",
    [adptc_result_k_console_fgets_error] = "fgets()",
    [adptc_result_k_proto_write_error] = "write()",
    [adptc_result_k_proto_partial_write] = "partial write()",
    [adptc_result_k_serial_getattr_error] = "tcgetattr()",
    [adptc_result_k_serial_setispeed_error] = "cfsetispeed()",
    [adptc_result_k_serial_setospeed_error] = "cfsetospeed()",
    [adptc_result_k_serial_setattr_error] = "tcsetattr()",
};

char const *adptc_result_kind_to_str(enum adptc_result_kind const kind) {
  assert(kind < RESULT_KIND_COUNT);
  assert(kind_to_str[kind]);

  return kind_to_str[kind];
}

void adptc_result_print(FILE *const out, struct adptc_result const res) {
  char const *const kind_str = adptc_result_kind_to_str(res.kind);
  assert(kind_str);

  fprintf(out, "%s", kind_str);
  if (res.os_code)
    fprintf(out, ": %s", strerror(res.os_code));
}
