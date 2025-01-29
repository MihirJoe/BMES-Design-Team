#ifndef ADAPT_CLIENT_INTERACT_H
#define ADAPT_CLIENT_INTERACT_H

struct adapt_client_interact_context {
  int serial_fd;
};

void adapt_client_interact_main(struct adapt_client_interact_context ctx);

#endif // ADAPT_CLIENT_INTERACT_H
