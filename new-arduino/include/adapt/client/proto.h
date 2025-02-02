#ifndef ADAPT_CLIENT_PROTO_H
#define ADAPT_CLIENT_PROTO_H

#include <adapt/client/result.h>

typedef void *adptc_proto_response_decoder;

union adptc_proto_response_body {
  struct {
    float value;
  } float_measurement;
};

struct adptc_proto_response {
  unsigned char status_code;
  union adptc_proto_response_body body;
};

[[nodiscard]] char const *adptc_proto_status_code_to_str(unsigned char code);

[[nodiscard]] unsigned char adptc_proto_build_request(unsigned char code,
                                                      unsigned char body);
struct adptc_result adptc_proto_try_send_request(int fd, unsigned char req);

[[nodiscard]] float adptc_proto_unmarshall_float(unsigned long marsh);

adptc_proto_response_decoder adptc_proto_create_response_decoder(void);
void adptc_proto_destroy_response_decoder(adptc_proto_response_decoder decoder);

struct adptc_proto_response const *
adptc_proto_feed_response_decoder(adptc_proto_response_decoder decoder,
                                  unsigned char byte);

#endif // ADAPT_CLIENT_PROTO_H
