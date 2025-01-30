#include "interact.h"

#include "proto.h"

#include <adapt/proto.h>

#include <unistd.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INTERACT_INPUT_BUF_SIZE 32

#define INTERACT_OK_RESULT                                                     \
  ((struct adptc_interact_result){.kind = adptc_interact_rk_ok})
#define INTERACT_OTHER_RESULT(variant)                                         \
  ((struct adptc_interact_result){.kind = adptc_interact_rk_##variant})

struct request_def {
  char const *name;
  unsigned char code;
};

static struct request_def const request_defs[] = {
    {.name = "ela8", .code = adpt_proto_rc_extend_lin_act_8},
    {.name = "ela512", .code = adpt_proto_rc_extend_lin_act_512},
    {.name = "sla", .code = adpt_proto_rc_stop_lin_act},
    {.name = "sm", .code = adpt_proto_rc_set_measuring},
    {.name = "rla8", .code = adpt_proto_rc_retract_lin_act_8},
    {.name = "rla512", .code = adpt_proto_rc_retract_lin_act_512},
};

char const *
adptc_interact_result_kind_to_str(enum adptc_interact_result_kind const kind) {
  switch (kind) {
  case adptc_interact_rk_ok:
    return "OK";
  case adptc_interact_rk_fgets_error:
    return "fgets() failed";
  default:
    return NULL;
  }
}

void adptc_interact_print_result(FILE *const out,
                                 struct adptc_interact_result const res) {
  char const *const kind_str = adptc_interact_result_kind_to_str(res.kind);
  assert(kind_str);

  fprintf(out, "%s", kind_str);
}

static void process_line(int const serial_fd, char const *const Line) {
  for (size_t I = 0; I < sizeof(request_defs) / sizeof(request_defs[0]); I++) {
    struct request_def const *const def = &request_defs[I];
    size_t const req_name_len = strlen(def->name);
    if (strncmp(Line, def->name, req_name_len) != 0)
      continue;

    unsigned long const req_body = strtoul(Line + req_name_len, NULL, 0);
    unsigned char const req = adptc_proto_build_request(def->code, req_body);
    adptc_proto_try_send_request(serial_fd, req);

    break;
  }
}

struct adptc_interact_result adptc_interact_loop(int const serial_fd) {
  char input_buf[INTERACT_INPUT_BUF_SIZE];
  while (fgets(input_buf, sizeof(input_buf), stdin))
    process_line(serial_fd, input_buf);

  if (ferror(stdin))
    return INTERACT_OTHER_RESULT(fgets_error);

  return INTERACT_OK_RESULT;
}
