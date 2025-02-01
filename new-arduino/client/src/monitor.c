#include "monitor.h"

#include "listen.h"
#include "proto.h"

#include <adapt/proto.h>

#include <assert.h>
#include <stdio.h>

void adptc_monitor(adptc_listen_handle const lhnd, void *user_ctx) {
  struct adptc_monitor_ctx *const ctx = user_ctx;
  assert(ctx);

  struct adptc_proto_response_decoder resp_decdr;
  adptc_proto_init_response_decoder(&resp_decdr);

  struct adptc_listen_incoming const *incom;
  while ((incom = adptc_listen_accept_incoming(lhnd))) {
    for (size_t i = 0; i < incom->data_len; i++) {
      struct adptc_proto_response const *const resp =
          adptc_proto_feed_response_decoder(&resp_decdr, incom->data_buf[i]);
      if (!resp)
        continue;

      fprintf(ctx->file, "%s\n",
              adptc_proto_status_code_to_str(resp->status_code));
      // TODO: print body
    }
  }
}
