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
#include <string.h>

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

  atomic_flag continue_listening_flag = ATOMIC_FLAG_INIT;
  atomic_flag_test_and_set(&continue_listening_flag);

  struct adptc_listen_conn listen_conn = ADPTC_LISTEN_CONN_INIT;
  adptc_listen_open_conn(&listen_conn);

  struct adptc_listen_ctx listen_ctx = {.continue_flag =
                                            &continue_listening_flag,
                                        .conn = &listen_conn,
                                        .serial_fd = serial_fd};

  pthread_t listener_thread;
  if (pthread_create(&listener_thread, NULL, adptc_listen_main, &listen_ctx) !=
      0) {
    fprintf(stderr, "fatal error: cannot spawn listener thread\n");
    close(serial_fd);
    return EXIT_FAILURE;
  }

  atomic_flag continue_monitoring_flag = ATOMIC_FLAG_INIT;
  atomic_flag_test_and_set(&continue_monitoring_flag);

  struct adptc_monitor_ctx display_ctx = {.continue_flag =
                                              &continue_monitoring_flag,
                                          .conn = &listen_conn,
                                          .file = monitor_file};

  pthread_t monitor_thread;
  if (pthread_create(&monitor_thread, NULL, adptc_monitor_main, &display_ctx) !=
      0) {
    fprintf(stderr, "fatal error: cannot spawn monitor thread\n");
    close(serial_fd);
    return EXIT_FAILURE;
  }

  aptc_res = adptc_console_attend(serial_fd);
  if (!ADPTC_RESULT_IS_OK(aptc_res)) {
    fprintf(stderr, "fatal error: ");
    adptc_result_print(stderr, aptc_res);
    fprintf(stderr, "\n");
    adptc_listen_close_conn(&listen_conn);
    close(serial_fd);
    return EXIT_FAILURE;
  }

  atomic_flag_clear(&continue_monitoring_flag);
  atomic_flag_clear(&continue_listening_flag);

  pthread_join(monitor_thread, NULL);
  pthread_join(listener_thread, NULL);

  // Clean up resources.
  adptc_listen_close_conn(&listen_conn);
  fclose(monitor_file);
  close(serial_fd);

  return EXIT_SUCCESS;
}
