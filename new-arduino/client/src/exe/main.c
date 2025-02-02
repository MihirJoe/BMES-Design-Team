#include "cli.h"
#include "console.h"
#include "monitor.h"

#include <adapt/client/listener.h>
#include <adapt/client/result.h>
#include <adapt/client/serial.h>

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SERIAL_PATH "/dev/ttyACM0"

int main(int const argc, char *argv[const]) {
  struct adptc_cli_config cli_config;
  adptc_cli_parse_args(argc, argv, &cli_config);

  int const serial_fd = open(cli_config.serial_path, O_RDWR);
  if (serial_fd == -1) {
    fprintf(stderr, "fatal error: cannot open serial TTY: %s\n",
            cli_config.serial_path);
    return EXIT_FAILURE;
  }

  FILE *const monitor_file = fopen(cli_config.monitor_path, "w");
  if (!monitor_file) {
    fprintf(stderr, "fatal error: cannot open monitor file: %s\n",
            cli_config.monitor_path);
    return EXIT_FAILURE;
  }

  adptc_serial_try_configure(serial_fd);

  adptc_monitor const monitor = adptc_monitor_create(monitor_file);
  adptc_listener const listener = adptc_listener_create();
  adptc_listener_start(listener, serial_fd, adptc_monitor_callback, monitor);

  adptc_console_try_attend(serial_fd);

  adptc_listener_stop(listener);
  adptc_listener_destroy(listener);
  adptc_monitor_destroy(monitor);
  fclose(monitor_file);
  close(serial_fd);

  return EXIT_SUCCESS;
}
