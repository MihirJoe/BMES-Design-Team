#ifndef ADAPT_CLIENT_TTY_H
#define ADAPT_CLIENT_TTY_H

#include <stdio.h>

#define ADAPT_CLIENT_TTY_IS_OK(res) (res.kind == adapt_client_tty_RK_Ok)
#define ADAPT_CLIENT_TTY_IS_ERROR(res) (res.kind != adapt_client_tty_RK_Ok)

enum adapt_client_tty_result_kind {
  adapt_client_tty_RK_Ok,
  adapt_client_tty_RK_GetAttrError,
  adapt_client_tty_RK_SetInSpeedError,
  adapt_client_tty_RK_SetOutSpeedError,
  adapt_client_tty_RK_SetAttrError,
};

struct adapt_client_tty_result {
  enum adapt_client_tty_result_kind kind;
  int code;
};

char const *adapt_client_tty_message_for_result_kind(
    enum adapt_client_tty_result_kind kind);
void adapt_client_tty_print_result(FILE *out,
                                   struct adapt_client_tty_result result);
struct adapt_client_tty_result adapt_client_tty_try_configure(int fd);

#endif // ADAPT_CLIENT_TTY_H
