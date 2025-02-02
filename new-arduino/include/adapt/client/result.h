#ifndef ADAPT_CLIENT_RESULT_H
#define ADAPT_CLIENT_RESULT_H

#include <errno.h>
#include <stdio.h>

#define ADPTC_RESULT_IS_OK(res) ((res).kind == adptc_result_k_ok)

#define ADPTC_OK_RESULT                                                        \
  ((struct adptc_result){.kind = adptc_result_k_ok, .os_code = 0})
#define ADPTC_OS_RESULT(variant)                                               \
  ((struct adptc_result){.kind = (adptc_result_k_##variant), .os_code = errno})
#define ADPTC_OTHER_RESULT(variant)                                            \
  ((struct adptc_result){.kind = (adptc_result_k_##variant), .os_code = 0})

enum adptc_result_kind {
  adptc_result_k_ok,
  adptc_result_k_console_fgets_error,
  adptc_result_k_listen_init_mutex_error,
  adptc_result_k_listen_init_cond_error,
  adptc_result_k_listen_destroy_mutex_error,
  adptc_result_k_listen_destroy_cond_error,
  adptc_result_k_proto_write_error,
  adptc_result_k_proto_partial_write,
  adptc_result_k_serial_getattr_error,
  adptc_result_k_serial_setispeed_error,
  adptc_result_k_serial_setospeed_error,
  adptc_result_k_serial_setattr_error,
};

struct adptc_result {
  enum adptc_result_kind kind;
  int os_code;
};

[[nodiscard]] char const *adptc_result_kind_to_str(enum adptc_result_kind kind);
void adptc_result_print(FILE *out, struct adptc_result res);

#endif // ADAPT_CLIENT_RESULT_H
