#include "tty.h"

#include <adapt/proto.h>

#include <termios.h>

#include <errno.h>
#include <string.h>

#if _POSIX_C_SOURCE < 200112l
#error "SUSv3 required"
#endif

// Evil preprocessor hacks!!!
#define TTY_PP_GLUE(a, b) a##b
#define TTY_PP_CALL(f, ...) f(__VA_ARGS__)
#define TTY_SPEED TTY_PP_CALL(TTY_PP_GLUE, B, ADAPT_SERIAL_BAUD)

#define TTY_OK_RESULT                                                          \
  ((struct adapt_client_tty_result){.kind = adapt_client_tty_RK_Ok, .code = 0})
#define TTY_OS_ERROR_RESULT(variant)                                           \
  ((struct adapt_client_tty_result){.kind = adapt_client_tty_RK_##variant,     \
                                    .code = errno})

char const *adapt_client_tty_message_for_result_kind(
    enum adapt_client_tty_result_kind const kind) {
  switch (kind) {
  case adapt_client_tty_RK_Ok:
    return "OK";
  case adapt_client_tty_RK_GetAttrError:
    return "tcgetattr() failed";
  case adapt_client_tty_RK_SetInSpeedError:
    return "cfsetispeed() failed";
  case adapt_client_tty_RK_SetOutSpeedError:
    return "cfsetospeed() failed";
  case adapt_client_tty_RK_SetAttrError:
    return "tcsetattr() failed";
  default:
    return "";
  }
}

void adapt_client_tty_print_result(
    FILE *const out, struct adapt_client_tty_result const result) {
  fprintf(out, "%s", adapt_client_tty_message_for_result_kind(result.kind));
  if (result.code != 0)
    fprintf(out, ": %s", strerror(result.code));

  fprintf(out, "\n");
}

static struct adapt_client_tty_result try_get_attr(int const fd,
                                                   struct termios *const tty) {
  if (tcgetattr(fd, tty) == -1)
    return TTY_OS_ERROR_RESULT(GetAttrError);

  return TTY_OK_RESULT;
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

static struct adapt_client_tty_result try_set_speed(struct termios *const tty) {
  if (cfsetispeed(tty, TTY_SPEED) == -1)
    return TTY_OS_ERROR_RESULT(SetInSpeedError);
  if (cfsetospeed(tty, TTY_SPEED) == -1)
    return TTY_OS_ERROR_RESULT(SetOutSpeedError);

  return TTY_OK_RESULT;
}

static struct adapt_client_tty_result try_set_attr(int const fd,
                                                   struct termios *const tty) {
  if (tcsetattr(fd, TCSANOW, tty) == -1)
    return TTY_OS_ERROR_RESULT(SetAttrError);

  // From `man 3 termios`:
  //   Note that tcsetattr() returns success if any of the requested
  //   changes could be successfully carried out. Therefore, when
  //   making multiple changes it may be necessary to follow this call
  //   with a further call to tcgetattr() to check that all changes
  //   have been performed successfully.

  // Because this is a simple, one-off client program,
  // we will not perform any such verification.

  return TTY_OK_RESULT;
}

struct adapt_client_tty_result adapt_client_tty_try_configure(int const fd) {
  struct adapt_client_tty_result result = TTY_OK_RESULT;

  struct termios tty;
  result = try_get_attr(fd, &tty);
  if (ADAPT_CLIENT_TTY_IS_ERROR(result))
    goto done;

  configure_input_modes(&tty.c_iflag);
  configure_output_modes(&tty.c_oflag);
  configure_control_modes(&tty.c_cflag);
  configure_local_modes(&tty.c_lflag);
  configure_special_chars(tty.c_cc);

  result = try_set_speed(&tty);
  if (ADAPT_CLIENT_TTY_IS_ERROR(result))
    goto done;

  result = try_set_attr(fd, &tty);
  if (ADAPT_CLIENT_TTY_IS_ERROR(result))
    goto done;

done:
  return result;
}
