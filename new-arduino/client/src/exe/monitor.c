#include "monitor.h"

#include <adapt/client/listener.h>
#include <adapt/client/proto.h>
#include <adapt/proto.h>

#include <assert.h>
#include <stdio.h>

void adptc_monitor(adptc_listener_receiver const rcvr, void *user_ctx) {
  struct adptc_monitor_ctx *const ctx = user_ctx;

  struct adptc_proto_response_decoder resp_decdr;
  adptc_proto_init_response_decoder(&resp_decdr);

  adptc_listener_incoming icmg;
  while ((icmg = adptc_listener_accept_incoming(rcvr))) {
    unsigned char const *const incom_data =
        adptc_listener_get_incoming_data(icmg);
    size_t const incom_data_len = adptc_listener_get_incoming_data_len(icmg);
    double const incom_fill_ratio =
        adptc_listener_get_incoming_fill_ratio(icmg);

    fprintf(ctx->file, "fill: %lf%%\n", incom_fill_ratio);

    for (size_t i = 0; i < incom_data_len; i++) {
      struct adptc_proto_response const *const resp =
          adptc_proto_feed_response_decoder(&resp_decdr, incom_data[i]);
      if (!resp)
        continue;

      fprintf(ctx->file, "%s\n",
              adptc_proto_status_code_to_str(resp->status_code));
      // TODO: print body
    }
  }
}
