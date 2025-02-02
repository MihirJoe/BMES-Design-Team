/// \file

#ifndef ADAPT_CLIENT_CLI_H
#define ADAPT_CLIENT_CLI_H

struct adptc_cli_config {
  char const *serial_path;
  char const *monitor_path;
};

void adptc_cli_parse_args(int argc, char *argv[],
                          struct adptc_cli_config *config);

#endif // ADAPT_CLIENT_CLI_H
