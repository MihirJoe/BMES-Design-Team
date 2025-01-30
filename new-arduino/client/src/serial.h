/// \brief ADAPT USB-serial TTY control.

#ifndef ADAPT_CLIENT_SERIAL_H
#define ADAPT_CLIENT_SERIAL_H

#include <adapt/proto.h>

#include <stdio.h>

#define ADPTC_SERIAL_IS_OK(res) (res.kind == adptc_serial_RK_Ok)
#define ADPTC_SERIAL_IS_ERROR(res) (res.kind != adptc_serial_RK_Ok)

enum adptc_serial_result_kind {
  adptc_serial_RK_Ok,
  adptc_serial_RK_GetAttrError,
  adptc_serial_RK_SetInSpeedError,
  adptc_serial_RK_SetOutSpeedError,
  adptc_serial_RK_SetAttrError,
};

struct adptc_serial_result {
  enum adptc_serial_result_kind kind;
  int code;
};

char const *adptc_serial_result_kind_to_str(enum adptc_serial_result_kind kind);
void adptc_serial_print_result(FILE *out, struct adptc_serial_result res);
struct adptc_serial_result adptc_serial_try_configure(int fd);

#endif // ADAPT_CLIENT_SERIAL_H
