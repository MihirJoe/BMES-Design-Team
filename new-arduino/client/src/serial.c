#include "serial.h"

#include <adapt/proto.h>

#include <termios.h>

#include <assert.h>
#include <errno.h>
#include <string.h>

#if _POSIX_C_SOURCE < 200112l
#error "SUSv3 required"
#endif

#if ADPT_PROTO_SERIAL_BAUD == 115200l
#define SERIAL_SPEED B115200
#else
#error "unsupported serial baud"
#endif

#define SERIAL_OK_RESULT                                                       \
  ((struct adptc_serial_result){.kind = adptc_serial_RK_Ok, .code = 0})
#define SERIAL_OS_ERROR_RESULT(variant)                                        \
  ((struct adptc_serial_result){.kind = adptc_serial_RK_##variant,             \
                                .code = errno})

char const *
adptc_serial_result_kind_to_str(enum adptc_serial_result_kind const kind) {
  switch (kind) {
  case adptc_serial_RK_Ok:
    return "OK";
  case adptc_serial_RK_GetAttrError:
    return "tcgetattr() failed";
  case adptc_serial_RK_SetInSpeedError:
    return "cfsetispeed() failed";
  case adptc_serial_RK_SetOutSpeedError:
    return "cfsetospeed() failed";
  case adptc_serial_RK_SetAttrError:
    return "tcsetattr() failed";
  default:
    return NULL;
  }
}

void adptc_serial_print_result(FILE *const out,
                               struct adptc_serial_result const res) {
  char const *const kind_str = adptc_serial_result_kind_to_str(res.kind);
  assert(kind_str);

  fprintf(out, "%s", kind_str);
  if (res.code != 0)
    fprintf(out, ": %s", strerror(res.code));
}

static struct adptc_serial_result try_get_attr(int const fd,
                                               struct termios *const tty) {
  if (tcgetattr(fd, tty) == -1)
    return SERIAL_OS_ERROR_RESULT(GetAttrError);

  return SERIAL_OK_RESULT;
}

static void configure_input_modes(tcflag_t *const iflag) {
  // TODO: what about frame and parity errors?

  // Disable input manipulation.
  *iflag &= ~(ISTRIP | INLCR | IGNCR | IXON | IXOFF);
}

static void configure_output_modes(tcflag_t *const oflag) {
  // Disable output manipulation.
  *oflag &= ~(OPOST | ONLCR | OCRNL);
}

static void configure_control_modes(tcflag_t *const cflag) {
  // The character size is 8 bits.
  *cflag &= ~CSIZE;
  *cflag |= CS8;
  // Use one stop bit.
  *cflag &= ~CSTOPB;
  // Enable the receiver.
  *cflag |= CREAD;

  // TODO: what about parity checking?
  *cflag &= ~PARENB;

  // Ignore modem control lines.
  *cflag |= CLOCAL;
}

static void configure_local_modes(tcflag_t *const lflag) {
  // Don't generate signals.
  *lflag &= ~ISIG;
  // Disable canonical mode.
  *lflag &= ~ICANON;
  // Don't echo input.
  *lflag &= ~ECHO;
}

static void configure_special_chars(cc_t cc[]) {
  // Impose no lower bound on the read amount...
  cc[VMIN] = 0;
  // ...but reads timeout after 100 ms.
  cc[VTIME] = 1;
}

static struct adptc_serial_result try_set_speed(struct termios *const tty) {
  if (cfsetispeed(tty, SERIAL_SPEED) == -1)
    return SERIAL_OS_ERROR_RESULT(SetInSpeedError);
  if (cfsetospeed(tty, SERIAL_SPEED) == -1)
    return SERIAL_OS_ERROR_RESULT(SetOutSpeedError);

  return SERIAL_OK_RESULT;
}

static struct adptc_serial_result try_set_attr(int const fd,
                                               struct termios *const tty) {
  if (tcsetattr(fd, TCSANOW, tty) == -1)
    return SERIAL_OS_ERROR_RESULT(SetAttrError);

  // From `man 3 termios`:
  //   Note that tcsetattr() returns success if any of the requested
  //   changes could be successfully carried out. Therefore, when
  //   making multiple changes it may be necessary to follow this call
  //   with a further call to tcgetattr() to check that all changes
  //   have been performed successfully.

  // Because this is a simple, one-off client program,
  // we will not perform any such verification.

  return SERIAL_OK_RESULT;
}

struct adptc_serial_result adptc_serial_try_configure(int const fd) {
  struct adptc_serial_result res = SERIAL_OK_RESULT;

  struct termios tty;
  res = try_get_attr(fd, &tty);
  if (ADPTC_SERIAL_IS_ERROR(res))
    goto done;

  configure_input_modes(&tty.c_iflag);
  configure_output_modes(&tty.c_oflag);
  configure_control_modes(&tty.c_cflag);
  configure_local_modes(&tty.c_lflag);
  configure_special_chars(tty.c_cc);

  res = try_set_speed(&tty);
  if (ADPTC_SERIAL_IS_ERROR(res))
    goto done;

  res = try_set_attr(fd, &tty);
  if (ADPTC_SERIAL_IS_ERROR(res))
    goto done;

done:
  return res;
}
