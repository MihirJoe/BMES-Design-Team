#include "console.h"

#include "proto.h"
#include "result.h"

#include <adapt/proto.h>

#include <unistd.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONSOLE_INPUT_BUF_SIZE 32

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

static struct request_def const *find_request_def(char const *const name) {
  size_t i = 0;
  while (i < sizeof(request_defs) / sizeof(request_defs[0])) {
    struct request_def const *const def = &request_defs[i];
    size_t const def_name_len = strlen(def->name);
    if (strncmp(name, def->name, def_name_len) == 0)
      return def;

    i++;
  }

  return NULL;
}

static void process_line(int const serial_fd, char const *const line) {
  struct request_def const *const def = find_request_def(line);
  if (!def) {
    printf("error: unknown request\n");
    return;
  }

  unsigned long const req_body = strtoul(line + strlen(def->name), NULL, 0);
  unsigned char const req = adptc_proto_build_request(def->code, req_body);
  // TODO: check error
  adptc_proto_try_send_request(serial_fd, req);
}

struct adptc_result adptc_console_attend(int const serial_fd) {
  char input_buf[CONSOLE_INPUT_BUF_SIZE];
  while (printf("> "), fgets(input_buf, sizeof(input_buf), stdin))
    process_line(serial_fd, input_buf);

  if (ferror(stdin))
    return ADPTC_OTHER_RESULT(console_fgets_error);

  return ADPTC_OK_RESULT;
}
