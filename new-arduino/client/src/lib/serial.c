/// \file

#include <adapt/client/serial.h>

#include <adapt/client/result.h>
#include <adapt/proto.h>

#include <termios.h>

#include <assert.h>

#if ADPT_PROTO_SERIAL_BAUD == 9600l
#define SERIAL_SPEED B9600
#elif ADPT_PROTO_SERIAL_BAUD == 115200l
#define SERIAL_SPEED B115200
#else
#error "unsupported serial baud"
#endif

/// \internal
static adptc_serial_configure_result try_get_attr(int const fd,
                                                  struct termios *const tty) {
  if (tcgetattr(fd, tty) == -1)
    return adptc_result_os_error(serial_configure, tcgetattr, tcgetattr);

  return adptc_result_ok(serial_configure);
}

/// \internal
static adptc_serial_configure_result try_set_attr(int const fd,
                                                  struct termios *const tty) {
  if (tcsetattr(fd, TCSANOW, tty) == -1)
    return adptc_result_os_error(serial_configure, tcsetattr, tcsetattr);

  // NOTE: from `man 3 termios`:
  //         Note that tcsetattr() returns success if any of the requested
  //         changes could be successfully carried out. Therefore, when
  //         making multiple changes it may be necessary to follow this call
  //         with a further call to tcgetattr() to check that all changes
  //         have been performed successfully.
  //
  //       Because this is a simple, one-off client program,
  //       we will not perform any such verification.

  return adptc_result_ok(serial_configure);
}

/// \internal
static void configure_input_modes(tcflag_t *const iflag) {
  // TODO: what about frame and parity errors?

  // Disable input manipulation.
  *iflag &= ~(ISTRIP | INLCR | IGNCR | IXON | IXOFF);
}

/// \internal
static void configure_output_modes(tcflag_t *const oflag) {
  // Disable output manipulation.
  *oflag &= ~(OPOST | ONLCR | OCRNL);
}

/// \internal
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

/// \internal
static void configure_local_modes(tcflag_t *const lflag) {
  // Don't generate signals.
  *lflag &= ~ISIG;
  // Disable canonical mode.
  *lflag &= ~ICANON;
  // Don't echo input.
  *lflag &= ~ECHO;
}

/// \internal
static void configure_special_chars(cc_t cc[const]) {
  // Impose no lower bound on the read amount...
  cc[VMIN] = 0;
  // ...but reads timeout after 100 ms.
  cc[VTIME] = 1;
}

/// \internal
static adptc_serial_configure_result try_set_speed(struct termios *const tty) {
  if (cfsetispeed(tty, SERIAL_SPEED) == -1)
    return adptc_result_os_error(serial_configure, cfsetispeed, cfsetispeed);
  if (cfsetospeed(tty, SERIAL_SPEED) == -1)
    return adptc_result_os_error(serial_configure, cfsetospeed, cfsetospeed);

  return adptc_result_ok(serial_configure);
}

adptc_serial_configure_result adptc_serial_try_configure(int const fd) {
  adptc_serial_configure_result res;

  struct termios tty;
  res = try_get_attr(fd, &tty);
  if (!adptc_result_is_ok(res))
    goto done;

  configure_input_modes(&tty.c_iflag);
  configure_output_modes(&tty.c_oflag);
  configure_control_modes(&tty.c_cflag);
  configure_local_modes(&tty.c_lflag);
  configure_special_chars(tty.c_cc);

  res = try_set_speed(&tty);
  if (!adptc_result_is_ok(res))
    goto done;

  res = try_set_attr(fd, &tty);
  if (!adptc_result_is_ok(res))
    goto done;

done:
  return res;
}
