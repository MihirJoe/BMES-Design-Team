#include "cli.h"

#include <adapt/client/support.h>

#include <getopt.h>

#include <assert.h>
#include <stddef.h>

static struct option const long_options[] = {
    {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
    {.name = "serial", .has_arg = required_argument, .flag = NULL, .val = 's'},
    {.name = "monitor", .has_arg = required_argument, .flag = NULL, .val = 'm'},
    {0}};

void adptc_cli_parse_args(int const argc, char *argv[const],
                          struct adptc_cli_config *const config) {
  config->serial_path = NULL;
  config->monitor_path = NULL;

  while (1) {
    int option_idx = 0;
    int const c = getopt_long(argc, argv, "", long_options, &option_idx);
    if (c == -1)
      break;

    switch (c) {
    case 'h': adptc_support_todo;
    case 's': config->serial_path = optarg; break;
    case 'm': config->monitor_path = optarg; break;
    default: adptc_support_todo;
    }
  }
}
