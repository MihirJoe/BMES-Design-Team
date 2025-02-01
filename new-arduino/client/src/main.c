#include "cli.h"
#include "console.h"
#include "listen.h"
#include "monitor.h"
#include "result.h"
#include "serial.h"

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

  struct adptc_result aptc_res;

  aptc_res = adptc_serial_try_configure(serial_fd);
  if (!ADPTC_RESULT_IS_OK(aptc_res)) {
    fprintf(stderr, "fatal error: ");
    adptc_result_print(stderr, aptc_res);
    fprintf(stderr, "\n");
    close(serial_fd);
    return EXIT_FAILURE;
  }

  struct adptc_listen_sys listen_sys = ADPTC_LISTEN_SYS_INIT;
  struct adptc_monitor_ctx mon_user_ctx = {.file = monitor_file};

  adptc_listen_start(&listen_sys, serial_fd, adptc_monitor, &mon_user_ctx);

  aptc_res = adptc_console_attend(serial_fd);
  if (!ADPTC_RESULT_IS_OK(aptc_res)) {
    fprintf(stderr, "fatal error: ");
    adptc_result_print(stderr, aptc_res);
    fprintf(stderr, "\n");
    close(serial_fd);
    return EXIT_FAILURE;
  }

  adptc_listen_stop(&listen_sys);

  // Clean up resources.
  fclose(monitor_file);
  close(serial_fd);

  return EXIT_SUCCESS;
}
