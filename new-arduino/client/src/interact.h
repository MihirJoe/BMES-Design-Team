#ifndef ADAPT_CLIENT_INTERACT_H
#define ADAPT_CLIENT_INTERACT_H

#include <stdio.h>

#define ADPTC_INTERACT_IS_OK(res) (res.kind == adptc_interact_RK_Ok)
#define ADPTC_INTERACT_IS_ERROR(res) (res.kind != adptc_interact_RK_Ok)

enum adptc_interact_result_kind {
  adptc_interact_RK_Ok,
  adptc_interact_RK_FgetsError,
};

struct adptc_interact_result {
  enum adptc_interact_result_kind kind;
};

char const *
adptc_interact_result_kind_to_str(enum adptc_interact_result_kind kind);
void adptc_interact_print_result(FILE *out, struct adptc_interact_result res);
struct adptc_interact_result adptc_interact_loop(int serial_fd);

#endif // ADAPT_CLIENT_INTERACT_H
