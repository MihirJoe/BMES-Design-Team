#ifndef ADAPT_CLIENT_PROTO_H
#define ADAPT_CLIENT_PROTO_H

#include <adapt/client/result.h>

union adptc_proto_response_body {
  struct {
    float value;
  } float_measurement;
};

struct adptc_proto_response {
  unsigned char status_code;
  union adptc_proto_response_body body;
};

struct adptc_proto_response_decoder {
  struct adptc_proto_response resp;
  unsigned long marsh_float;
  int rem_float_bytes;
};

[[nodiscard]] char const *adptc_proto_status_code_to_str(unsigned char code);

[[nodiscard]] unsigned char adptc_proto_build_request(unsigned char code,
                                                      unsigned char body);
struct adptc_result adptc_proto_try_send_request(int fd, unsigned char req);

[[nodiscard]] float adptc_proto_unmarshall_float(unsigned long marsh);

void adptc_proto_init_response_decoder(
    struct adptc_proto_response_decoder *decdr);
struct adptc_proto_response const *
adptc_proto_feed_response_decoder(struct adptc_proto_response_decoder *decdr,
                                  unsigned char byte);

#endif // ADAPT_CLIENT_PROTO_H
