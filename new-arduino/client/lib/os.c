/// \file

#include <adapt/client/os.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

void adptc_os_print_error(FILE *out, struct adptc_os_error const e) {
  assert(out);
  assert(e.func_name);

  fprintf(out, "%s(): %s (%d)", e.func_name, strerror(e.code), e.code);
}
