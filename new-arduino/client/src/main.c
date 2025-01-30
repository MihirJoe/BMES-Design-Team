#include "interact.h"
#include "listen.h"
#include "serial.h"

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SERIAL_PATH "/dev/ttyACM0"

int main(int argc, char *argv[]) {
  char const *const serial_path = argc >= 2 ? argv[1] : DEFAULT_SERIAL_PATH;

  int const serial_fd = open(serial_path, O_RDWR);
  if (serial_fd == -1) {
    fprintf(stderr, "error: cannot open: %s\n", serial_path);
    return EXIT_FAILURE;
  }

  struct adptc_serial_result const serial_result =
      adptc_serial_try_configure(serial_fd);
  if (ADPTC_SERIAL_IS_ERROR(serial_result)) {
    adptc_serial_print_result(stderr, serial_result);
    close(serial_fd);
    return EXIT_FAILURE;
  }

  atomic_flag continue_listening_flag = ATOMIC_FLAG_INIT;
  atomic_flag_test_and_set(&continue_listening_flag);

  struct adptc_listen_ctx listen_ctx = {
      .serial_fd = serial_fd, .continue_flag = &continue_listening_flag};

  pthread_t listen_thread;
  if (pthread_create(&listen_thread, NULL, adptc_listen_main, &listen_ctx) !=
      0) {
    fprintf(stderr, "error: cannot spawn listener thread\n");
    close(serial_fd);
    return EXIT_FAILURE;
  }

  struct adptc_interact_result const interact_result =
      adptc_interact_loop(serial_fd);
  if (ADPTC_INTERACT_IS_ERROR(interact_result)) {
    adptc_interact_print_result(stderr, interact_result);
    close(serial_fd);
    return EXIT_FAILURE;
  }

  pthread_join(listen_thread, NULL);
  close(serial_fd);

  return EXIT_SUCCESS;
}
