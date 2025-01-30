/// \brief ADAPT USB-serial TTY control.

#ifndef ADAPT_CLIENT_SERIAL_H
#define ADAPT_CLIENT_SERIAL_H

#include <adapt/proto.h>

#include <stdio.h>

#define ADPTC_SERIAL_IS_OK(res) (res.kind == adptc_serial_rk_ok)
#define ADPTC_SERIAL_IS_ERROR(res) (res.kind != adptc_serial_rk_ok)

enum adptc_serial_result_kind {
  adptc_serial_rk_ok,
  adptc_serial_rk_getattr_error,
  adptc_serial_rk_setispeed_error,
  adptc_serial_rk_setospeed_error,
  adptc_serial_rk_setattr_error,
};

struct adptc_serial_result {
  enum adptc_serial_result_kind kind;
  int code;
};

char const *adptc_serial_result_kind_to_str(enum adptc_serial_result_kind kind);
void adptc_serial_print_result(FILE *out, struct adptc_serial_result res);
struct adptc_serial_result adptc_serial_try_configure(int fd);

#endif // ADAPT_CLIENT_SERIAL_H
