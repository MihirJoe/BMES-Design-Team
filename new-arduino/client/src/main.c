#include "interact.h"
#include "serial.h"
#include "tty.h"

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

  struct adapt_client_tty_result tty_result =
      adapt_client_tty_try_configure(serial_fd);
  if (ADAPT_CLIENT_TTY_IS_ERROR(tty_result)) {
    adapt_client_tty_print_result(stderr, tty_result);
    close(serial_fd);
    return EXIT_FAILURE;
  }

  atomic_flag continue_serial_flag = ATOMIC_FLAG_INIT;
  atomic_flag_test_and_set(&continue_serial_flag);

  struct adapt_client_serial_context serial_ctx = {
      .fd = serial_fd, .continue_flag = &continue_serial_flag};

  pthread_t serial_thread;
  if (pthread_create(&serial_thread, NULL, adapt_client_serial_main,
                     &serial_ctx) != 0) {
    fprintf(stderr, "error: cannot spawn serial thread\n");
    close(serial_fd);
    return EXIT_FAILURE;
  }

  struct adapt_client_interact_context interact_ctx = {.serial_fd = serial_fd};
  adapt_client_interact_main(interact_ctx);

  int const ExitCode = ferror(stdin) ? EXIT_FAILURE : EXIT_SUCCESS;

  pthread_join(serial_thread, NULL);
  close(serial_fd);

  return ExitCode;
}
