/// \file

#include "monitor.h"

#include <adapt/client/listener.h>
#include <adapt/client/proto.h>
#include <adapt/client/result.h>
#include <adapt/proto.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct monitor {
  FILE *file;
  adptc_proto_response_decoder resp_decdr;
};

adptc_monitor_create_result
adptc_monitor_try_create(FILE *const file,
                         adptc_proto_response_decoder const resp_decdr) {
  struct monitor *const montr = malloc(sizeof(struct monitor));
  if (!montr)
    return adptc_result_os_error(adptc_monitor_create, alloc_monitor, malloc);

  montr->file = file;
  montr->resp_decdr = resp_decdr;

  return adptc_result_ok_with(adptc_monitor_create, montr);
}

static void check_monitor(struct monitor *const montr) {
  assert(montr);
  assert(montr->file);
  assert(montr->resp_decdr);
}

void adptc_monitor_destroy(adptc_monitor const mhnd) {
  struct monitor *const montr = mhnd;
  check_monitor(montr);

  adptc_proto_response_decoder_destroy(montr->resp_decdr);
  free(montr);
}

void adptc_monitor_callback(adptc_listener_incoming const icmg,
                            void *user_ctx) {
  struct monitor *const montr = user_ctx;
  check_monitor(montr);

  unsigned char const *const icmg_data = adptc_listener_incoming_get_ptr(icmg);
  size_t const icmg_data_len = adptc_listener_incoming_get_len(icmg);

  for (size_t i = 0; i < icmg_data_len; i++) {
    struct adptc_proto_response const *const resp =
        adptc_proto_response_decoder_feed(&montr->resp_decdr, icmg_data[i]);
    if (!resp)
      continue;

    fprintf(montr->file, "%s\n",
            adptc_proto_status_code_to_str(resp->status_code));
    // TODO: print body
  }
}
