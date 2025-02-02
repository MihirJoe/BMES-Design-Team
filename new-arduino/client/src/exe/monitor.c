#include "monitor.h"

#include <adapt/client/listener.h>
#include <adapt/client/proto.h>
#include <adapt/proto.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct monitor {
  FILE *file;
  adptc_proto_response_decoder resp_decdr;
};

adptc_monitor adptc_monitor_create(FILE *const file) {
  struct monitor *const montr = malloc(sizeof(struct monitor));
  if (!montr)
    return NULL;

  montr->file = file;

  montr->resp_decdr = adptc_proto_create_response_decoder();
  if (!montr->resp_decdr) {
    free(montr);
    return NULL;
  }

  return montr;
}

static void check_monitor(struct monitor *const montr) {
  assert(montr);
  assert(montr->file);
  assert(montr->resp_decdr);
}

void adptc_monitor_destroy(adptc_monitor const mhnd) {
  struct monitor *const montr = mhnd;
  check_monitor(montr);

  adptc_proto_destroy_response_decoder(montr->resp_decdr);
  free(montr);
}

void adptc_monitor_callback(adptc_listener_incoming const icmg,
                            void *user_ctx) {
  struct monitor *const montr = user_ctx;
  check_monitor(montr);

  unsigned char const *const incom_data =
      adptc_listener_get_incoming_data(icmg);
  size_t const incom_data_len = adptc_listener_get_incoming_data_len(icmg);
  double const incom_fill_ratio = adptc_listener_get_incoming_fill_ratio(icmg);

  fprintf(montr->file, "fill: %lf%%\n", incom_fill_ratio);

  for (size_t i = 0; i < incom_data_len; i++) {
    struct adptc_proto_response const *const resp =
        adptc_proto_feed_response_decoder(&montr->resp_decdr, incom_data[i]);
    if (!resp)
      continue;

    fprintf(montr->file, "%s\n",
            adptc_proto_status_code_to_str(resp->status_code));
    // TODO: print body
  }
}
