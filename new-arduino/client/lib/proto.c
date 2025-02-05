/// \file

#include <adapt/client/proto.h>

#include <adapt/client/result.h>
#include <adapt/client/support.h>
#include <adapt/proto.h>

#include <unistd.h>

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

struct response_decoder {
  struct adptc_proto_response resp;
  unsigned long marsh_float;
  int rem_float_bytes;
};

static char const *const status_code_to_str[256] = {
    [adpt_proto_sc_force_measurement] = "force measurement.",
    [adpt_proto_sc_request_received] = "request received.",
    [adpt_proto_sc_done] = "done.",
    [adpt_proto_sc_ready] = "ready.",
    [adpt_proto_sc_setting_up_lin_act] = "setting up linear actuator...",
    [adpt_proto_sc_setting_up_hx711] = "setting up HX711...",
};

[[nodiscard]] char const *
adptc_proto_status_code_to_str(unsigned char const code) {
  char const *s = status_code_to_str[code & 0xff];
  return s ? s : "(unknown status)";
}

[[nodiscard]] unsigned char
adptc_proto_build_request(unsigned char const code, unsigned char const body) {
  return (code << 5) | (body & 0x1f);
}

adptc_proto_send_request_result
adptc_proto_try_send_request(int const fd, unsigned char const req) {
  ssize_t const write_res = write(fd, &req, 1);
  if (write_res != 1)
    return adptc_result_os_error(proto_send_request, write, write);

  return adptc_result_ok(proto_send_request);
}

[[nodiscard]] float adptc_proto_unmarshall_float(unsigned long const marsh) {
  // NOTE: this is quite explicit
  //       because the C Standard
  //       does not require `float` to be IEEE-754 `binary32`.

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

adptc_proto_response_decoder adptc_proto_response_decoder_create(void) {
  struct response_decoder *const decdr =
      malloc(sizeof(struct response_decoder));
  if (!decdr)
    return NULL;

  decdr->marsh_float = 0;
  decdr->rem_float_bytes = 0;

  return decdr;
}

static void check_response_decoder(struct response_decoder *const decdr) {
  assert(decdr);
}

void adptc_proto_response_decoder_destroy(
    adptc_proto_response_decoder const dhnd) {
  struct response_decoder *const decdr = dhnd;
  check_response_decoder(decdr);

  free(decdr);
}

struct adptc_proto_response const *
adptc_proto_response_decoder_feed(adptc_proto_response_decoder const dhnd,
                                  unsigned char const byte) {
  struct response_decoder *const decdr = dhnd;
  check_response_decoder(decdr);

  if (decdr->rem_float_bytes > 0) {
    decdr->marsh_float <<= 8;
    decdr->marsh_float |= byte;
    decdr->rem_float_bytes--;

    if (decdr->rem_float_bytes > 0)
      return NULL;

    decdr->resp.body.float_measurement.value =
        adptc_proto_unmarshall_float(decdr->marsh_float);

    return &decdr->resp;
  }

  unsigned char const status_code = byte;
  decdr->resp.status_code = status_code;

  if (status_code != adpt_proto_sc_force_measurement)
    return &decdr->resp;

  decdr->marsh_float = 0;
  decdr->rem_float_bytes = 4;

  return NULL;
}
