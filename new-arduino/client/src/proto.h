#ifndef ADAPT_CLIENT_PROTO_H
#define ADAPT_CLIENT_PROTO_H

#include <adapt/proto.h>

#include <stdio.h>

#define ADPTC_PROTO_IS_OK(res) (res.kind == adptc_proto_RK_Ok)
#define ADPTC_PROTO_IS_ERROR(res) (res.kind != adptc_proto_RK_Ok)

enum adptc_proto_result_kind {
  adptc_proto_RK_Ok,
  adptc_proto_RK_WriteError,
  adptc_proto_RK_PartialWrite,
};

struct adptc_proto_result {
  enum adptc_proto_result_kind kind;
  int code;
};

char const *adptc_proto_result_kind_to_str(enum adptc_proto_result_kind kind);
void adptc_proto_print_result(FILE *out, struct adptc_proto_result res);
char const *adptc_proto_status_code_to_str(unsigned char code);
unsigned char adptc_proto_build_request(unsigned char code, unsigned char body);
struct adptc_proto_result adptc_proto_try_send_request(int fd,
                                                       unsigned char req);
float adptc_proto_unmarshall_float(unsigned long marsh);

#endif // ADAPT_CLIENT_PROTO_H
