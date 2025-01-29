#include "serial.h"

#include <adapt/proto.h>

#include <unistd.h>

#include <assert.h>
#include <stdio.h>

#define SERIAL_BUF_LEN 256

static char *const message_for_response_status_code[256] = {
    [AdaptSC_Done] = "done.",
    [AdaptSC_Ready] = "ready.",
    [AdaptSC_SettingUpLinAct] = "setting up linear actuator...",
    [AdaptSC_SettingUpHx711] = "setting up HX711...",
    [AdaptSC_ForceMeasurement] = "force measurement available.",
    [AdaptSC_ReceivedRequest] = "received request.",
};

void *adapt_client_serial_main(void *const thread_arg) {
  struct adapt_client_serial_context const *const ctx = thread_arg;

  // This is basically a state machine.

  unsigned char SerialBuf[SERIAL_BUF_LEN];
  char FloatBuf[4];
  int RemFloatBytes = 0;
  ssize_t ReadSize = 0;
  ssize_t I = 0;

  while (atomic_flag_test_and_set(ctx->continue_flag)) {
    if (I >= ReadSize) {
      ReadSize = read(ctx->fd, SerialBuf, SERIAL_BUF_LEN);
      I = 0;
      continue;
    }

    if (RemFloatBytes > 0) {
      // We didn't finish unmarshalling a float.

      // Add another byte.
      FloatBuf[4 - RemFloatBytes] = SerialBuf[I];

      if (RemFloatBytes == 1) {

        // NOTE: this is super duper undefined behavior.
        float const Float = *(float *)FloatBuf;
        printf("%f\n", Float);
      }

      RemFloatBytes--;
    } else {
      unsigned char const StatusCode = SerialBuf[I];
      char *const StatusMsg = message_for_response_status_code[StatusCode];
      assert(StatusMsg);
      printf("%s\n", StatusMsg);

      if (StatusCode == AdaptSC_ForceMeasurement)
        RemFloatBytes = 4;
    }

    I++;
  }

  return NULL;
}
