#include "monitor.h"

#include "listen.h"
#include "proto.h"

#include <adapt/proto.h>

#include <assert.h>
#include <stdio.h>

void adptc_monitor(adptc_listen_sender const sender, void *user_ctx) {
  struct adptc_monitor_ctx *const ctx = user_ctx;
  assert(ctx);
  assert(ctx->file);

  struct adptc_proto_response_decoder resp_decdr;
  adptc_proto_init_response_decoder(&resp_decdr);

  adptc_listen_incoming incom;
  while ((incom = adptc_listen_accept_incoming(sender))) {
    unsigned char const *const incom_data =
        adptc_listen_get_incoming_data(incom);
    size_t const incom_data_len = adptc_listen_get_incoming_data_len(incom);
    double const incom_fill_ratio = adptc_listen_get_incoming_fill_ratio(incom);

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
