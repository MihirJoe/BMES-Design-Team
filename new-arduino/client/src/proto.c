#include "proto.h"

#include <unistd.h>

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define PROTO_OK_RESULT                                                        \
  ((struct adptc_proto_result){.kind = adptc_proto_RK_Ok, .code = 0})
#define PROTO_OS_ERROR_RESULT(variant)                                         \
  ((struct adptc_proto_result){.kind = adptc_proto_RK_##variant, .code = errno})
#define PROTO_OTHER_RESULT(variant)                                            \
  ((struct adptc_proto_result){.kind = adptc_proto_RK_##variant, .code = 0})

static char const *const status_code_to_str[256] = {
    [adpt_proto_SC_ForceMeasurement] = "force measurement.",
    [adpt_proto_SC_RequestReceived] = "request received.",
    [adpt_proto_SC_Done] = "done.",
    [adpt_proto_SC_Ready] = "ready.",
    [adpt_proto_SC_SettingUpLinAct] = "setting up linear actuator...",
    [adpt_proto_SC_SettingUpHx711] = "setting up HX711...",
};

char const *
adptc_proto_result_kind_to_str(enum adptc_proto_result_kind const kind) {
  switch (kind) {
  case adptc_proto_RK_Ok:
    return "OK";
  case adptc_proto_RK_WriteError:
    return "write() failed";
  case adptc_proto_RK_PartialWrite:
    return "partial write";
  default:
    return NULL;
  }
}

void adptc_proto_print_result(FILE *const out,
                              struct adptc_proto_result const res) {
  char const *const kind_str = adptc_proto_result_kind_to_str(res.kind);
  assert(kind_str);

  fprintf(out, "%s", kind_str);
  if (res.code != 0)
    fprintf(out, ": %s", strerror(res.code));
}

char const *adptc_proto_status_code_to_str(unsigned char const code) {
  return status_code_to_str[code];
}

unsigned char adptc_proto_build_request(unsigned char const code,
                                        unsigned char const body) {
  return (code << 5) | (body & 0x1f);
}

struct adptc_proto_result
adptc_proto_try_send_request(int const fd, unsigned char const req) {
  ssize_t const write_res = write(fd, &req, 1);
  if (write_res == -1)
    return PROTO_OS_ERROR_RESULT(WriteError);
  if (write_res == 0)
    return PROTO_OTHER_RESULT(PartialWrite);

  assert(write_res == 1);

  return PROTO_OK_RESULT;
}

float adptc_proto_unmarshall_float(unsigned long const marsh) {
  // This is quite explicit
  // because the C Standard does not require `float` to be IEEE-754 `binary32`.

  int const sign_bit = (marsh >> 31) & 1;
  int const sign = sign_bit ? -1 : 1;

  int const biased_exp = (marsh >> 23) & 0xff;
  int const exp = biased_exp - 127;

  long const mantissa = marsh & 0x7fffffl;
  float const epsilon = 1.f / (float)(1ul << 23);
  float const frac = mantissa * epsilon;

  float const coef = (biased_exp != 0) + frac;

  return biased_exp == 0xff ? (mantissa == 0 ? sign * INFINITY : NAN)
                            : sign * exp * coef;
}
