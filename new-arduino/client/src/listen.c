#include "listen.h"

#include "proto.h"

#include <adapt/proto.h>

#include <unistd.h>

#include <assert.h>
#include <stdio.h>

#define SERIAL_BUF_LEN 256

void *adptc_listen_main(void *const thread_arg) {
  struct adptc_listen_ctx const *const thread_ctx = thread_arg;

  // This is basically a state machine.

  unsigned char SerialBuf[SERIAL_BUF_LEN];
  unsigned long marsh_float = 0;
  int RemFloatBytes = 0;
  ssize_t ReadSize = 0;
  ssize_t I = 0;

  while (atomic_flag_test_and_set(thread_ctx->continue_flag)) {
    if (I >= ReadSize) {
      ReadSize = read(thread_ctx->serial_fd, SerialBuf, SERIAL_BUF_LEN);
      I = 0;
      continue;
    }

    if (RemFloatBytes > 0) {
      // We didn't finish unmarshalling a float.

      // Add another byte.
      marsh_float <<= 8;
      marsh_float |= SerialBuf[I];

      if (RemFloatBytes == 1)
        printf("%f\n", adptc_proto_unmarshall_float(marsh_float));

      RemFloatBytes--;
    } else {
      unsigned char const StatusCode = SerialBuf[I];
      char const *const StatusMsg = adptc_proto_status_code_to_str(StatusCode);
      assert(StatusMsg);
      printf("%s\n", StatusMsg);

      if (StatusCode == adpt_proto_SC_ForceMeasurement)
        RemFloatBytes = 4;
    }

    I++;
  }

  return NULL;
}
