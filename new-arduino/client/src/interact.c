#include "interact.h"

#include <adapt/proto.h>

#include <unistd.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INTERACT_INPUT_BUF_LEN 32

struct request_def {
  char const *name;
  unsigned char code;
};

static struct request_def const request_defs[] = {
    {.name = "ela16", .code = AdaptRC_ExtendLinAct16},
    {.name = "ela512", .code = AdaptRC_ExtendLinAct512},
    {.name = "sla", .code = AdaptRC_StopLinAct},
    {.name = "sm", .code = AdaptRC_SetMeasuring},
    {.name = "rla16", .code = AdaptRC_RetractLinAct16},
    {.name = "rla512", .code = AdaptRC_RetractLinAct512},
};

static void process_line(int const serial_fd, char const *const Line) {
  for (size_t I = 0; I < sizeof(request_defs) / sizeof(request_defs[0]); I++) {
    struct request_def const *const def = &request_defs[I];
    size_t const ReqNameLen = strlen(def->name);
    if (strncmp(Line, def->name, ReqNameLen) != 0)
      continue;

    unsigned long const ReqBody = strtoul(Line + ReqNameLen, NULL, 0);
    unsigned char Req = (def->code << 5) | (ReqBody & 0x1f);
    printf("request: %02x\n", Req);
    write(serial_fd, &Req, 1);

    break;
  }
}

void adapt_client_interact_main(
    struct adapt_client_interact_context const ctx) {
  char input_buf[INTERACT_INPUT_BUF_LEN];
  while (fgets(input_buf, sizeof(input_buf), stdin))
    process_line(ctx.serial_fd, input_buf);
}
