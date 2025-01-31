#include "monitor.h"

#include "listen.h"
#include "proto.h"

#include <adapt/proto.h>

#include <assert.h>
#include <stdio.h>

void *adptc_monitor_main(void *const thread_arg) {
  struct adptc_monitor_ctx const *const ctx = thread_arg;
  assert(ctx);
  assert(ctx->conn);

  struct adptc_proto_response_decoder resp_decdr;
  adptc_proto_init_response_decoder(&resp_decdr);

  adptc_listen_register_receiver(ctx->conn);

  while (atomic_flag_test_and_set(ctx->continue_flag)) {
    struct adptc_listen_incoming const *const incom =
        adptc_listen_accept_incoming(ctx->conn);
    if (!incom)
      continue;

    for (size_t i = 0; i < incom->data_len; i++) {
      struct adptc_proto_response const *const resp =
          adptc_proto_feed_response_decoder(&resp_decdr, incom->data_buf[i]);
      if (!resp)
        continue;

      fprintf(ctx->file, "%s\n",
              adptc_proto_status_code_to_str(resp->status_code));
      // TODO: print body
    }

    adptc_listen_end_inspection(ctx->conn);
  }

  adptc_listen_unregister_receiver(ctx->conn);

  return NULL;
}
