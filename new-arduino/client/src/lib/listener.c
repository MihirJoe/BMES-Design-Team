#include <adapt/client/listener.h>

#include <adapt/client/support.h>

#include <pthread.h>
#include <unistd.h>

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// NOTE: for a baud of 115200
//       (the maximum we will support),
//       the theoretical maximum number of bytes
//       the Arduino can transmit in one decisecond (100 ms)
//       is exactly 1440.
#define LISTENER_BUF_SIZE 1440

struct incoming {
  unsigned char data_buf[LISTENER_BUF_SIZE];
  size_t data_len;
};

struct channel {
  struct incoming icmg;
  unsigned char read_buf[LISTENER_BUF_SIZE];
  atomic_bool icmg_out_for_delivery;
  atomic_flag busy_flag;
  pthread_mutex_t icmg_lock;
  pthread_cond_t rcvr_has_work;
};

struct sender {
  struct channel *chan;
  int serial_fd;
  atomic_flag continue_flag;
};

struct receiver {
  struct channel *chan;
  adptc_listener_callback user_cb;
  void *user_ctx;
  atomic_flag continue_flag;
};

struct listener {
  struct channel chan;
  struct sender sndr;
  struct receiver rcvr;
  pthread_t sndr_thread;
  pthread_t rcvr_thread;
};

[[nodiscard]] adptc_listener adptc_listener_create(void) {
  struct listener *const lsnr = malloc(sizeof(struct listener));
  if (!lsnr)
    return NULL;

  lsnr->chan.busy_flag = (atomic_flag)ATOMIC_FLAG_INIT;
  lsnr->sndr.continue_flag = (atomic_flag)ATOMIC_FLAG_INIT;
  lsnr->rcvr.continue_flag = (atomic_flag)ATOMIC_FLAG_INIT;
  lsnr->sndr.chan = &lsnr->chan;
  lsnr->rcvr.chan = &lsnr->chan;

  return lsnr;
}

static void check_listener(struct listener *const lsnr) {
  assert(lsnr);
  assert(lsnr->sndr.chan == &lsnr->chan);
  assert(lsnr->rcvr.chan == &lsnr->chan);
}

void adptc_listener_destroy(adptc_listener lhnd) {
  struct listener *const lsnr = lhnd;
  check_listener(lsnr);

  free(lsnr);
}

static void check_sender(struct sender *const sndr) { assert(sndr); }

static void *sender_routine(void *const thread_arg) {
  struct sender *const sndr = thread_arg;
  check_sender(sndr);

  // NOTE: this is a finite state machine.

  enum state {
    state_read,
    state_wait,
    state_deliver,
  };

  int pthread_res;

  enum state state = state_read;
  ssize_t read_res;

  // NOTE: in-between states,
  //       check the 'continue' flag.
  //       When it is cleared, return.
  while (atomic_flag_test_and_set(&sndr->continue_flag)) {
    switch (state) {
    case state_read:
      // NOTE: we don't read directly into the 'incoming' structure
      //       because we plan to begin the next read
      //       while the receiver thread
      //       is processing the previous incoming data.
      read_res = read(sndr->serial_fd, sndr->chan->read_buf, LISTENER_BUF_SIZE);
      if (read_res <= 0)
        // TODO
        read_res = 0;

      state = state_wait;
      break;

    case state_wait:
      // Wait until the receiver thread is done using the 'incoming' structure
      // so that we can copy the read data into it.
      // NOTE: because the default state of the 'busy' flag is set,
      //       on the first iteration of this loop,
      //       we wait until the receiver thread
      //       calls adptc_listen_accept_incoming.
      //       This gives us confidence
      //       that the receiver thread
      //       will receive the condition variable signal.
      // NOTE: this is basically a spinlock.
      //       Condition variables are costly
      //       and we don't expect to be here very long.
      if (!atomic_flag_test_and_set(&sndr->chan->busy_flag))
        state = state_deliver;

      break;

    case state_deliver:
      // Copy the read data to the 'incoming' structure.
      // NOTE: this structure is protected not by a lock
      //       but by the 'busy' flag,
      //       which, when cleared,
      //       indicates that the receiver thread
      //       is not using the 'incoming' structure.
      size_t const read_len = read_res;
      memcpy(sndr->chan->icmg.data_buf, sndr->chan->read_buf, read_len);
      sndr->chan->icmg.data_len = read_len;

      // NOTE: Acquiring this lock
      //       is part of the song and dance of using condition variables.
      pthread_res = pthread_mutex_lock(&sndr->chan->icmg_lock);
      if (pthread_res != 0)
        adptc_support_todo;

      // Indicate to the receiver thread
      // that incoming serial data is available.
      atomic_store(&sndr->chan->icmg_out_for_delivery, true);

      // Release the lock
      // so that the receiver thread may wake up
      // when the condition variable is signalled.
      pthread_res = pthread_mutex_unlock(&sndr->chan->icmg_lock);
      if (pthread_res != 0)
        adptc_support_todo;

      // Wake up the receiver thread
      // (if it is waiting on the condition variable).
      pthread_res = pthread_cond_signal(&sndr->chan->rcvr_has_work);
      if (pthread_res != 0)
        adptc_support_todo;

      // While the receiver thread is processing the incoming data,
      // begin the next read.
      state = state_read;
      break;

    default: adptc_support_todo;
    }
  }

  return NULL;
}

static struct incoming *accept_incoming(struct receiver *const rcvr) {
  int pthread_res;

  // Tell the sender thread
  // that we are done using the previous incoming data.
  atomic_flag_clear(&rcvr->chan->busy_flag);

  bool should_continue;
  while ((should_continue = atomic_flag_test_and_set(&rcvr->continue_flag)) &&
         !atomic_load(&rcvr->chan->icmg_out_for_delivery)) {
    // Wait until someone tells us to wake up.
    // NOTE: waiting on a condition variable releases the lock,
    //       allowing the sender thread to acquire it
    //       and set the 'out-for-delivery' flag.
    pthread_res =
        pthread_cond_wait(&rcvr->chan->rcvr_has_work, &rcvr->chan->icmg_lock);
    if (pthread_res != 0)
      adptc_support_todo;
  }

  // The main thread has requested for us to exit.
  // Return `NULL` so that the caller knows we are done.
  if (!should_continue)
    return NULL;

  // Tell the sender thread that we received the delivery.
  atomic_store(&rcvr->chan->icmg_out_for_delivery, false);

  return &rcvr->chan->icmg;
}

static void check_receiver(struct receiver *const rcvr) {
  assert(rcvr);
  assert(rcvr->user_cb);
}

static void *receiver_routine(void *const thread_arg) {
  struct receiver *const rcvr = thread_arg;
  check_receiver(rcvr);

  int pthread_res;

  pthread_res = pthread_mutex_lock(&rcvr->chan->icmg_lock);
  if (pthread_res != 0)
    adptc_support_todo;

  struct incoming *icmg;
  while ((icmg = accept_incoming(rcvr)))
    rcvr->user_cb(rcvr, rcvr->user_ctx);

  pthread_res = pthread_mutex_unlock(&rcvr->chan->icmg_lock);
  if (pthread_res != 0)
    adptc_support_todo;

  return NULL;
}

void adptc_listener_start(adptc_listener const lhnd, int const serial_fd,
                          adptc_listener_callback const user_cb,
                          void *const user_ctx) {
  struct listener *const lsnr = lhnd;
  check_listener(lsnr);

  int pthread_res;

  atomic_store(&lsnr->chan.icmg_out_for_delivery, false);
  atomic_flag_test_and_set(&lsnr->chan.busy_flag);
  atomic_flag_test_and_set(&lsnr->sndr.continue_flag);
  atomic_flag_test_and_set(&lsnr->rcvr.continue_flag);

  pthread_mutexattr_t lock_attr;

  pthread_res = pthread_mutexattr_init(&lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  // TODO: check error
  pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_ERRORCHECK);

  pthread_res = pthread_mutex_init(&lsnr->chan.icmg_lock, &lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_mutexattr_destroy(&lock_attr);
  if (pthread_res != 0)
    adptc_support_todo;

  pthread_res = pthread_cond_init(&lsnr->chan.rcvr_has_work, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  lsnr->sndr.serial_fd = serial_fd;
  pthread_res =
      pthread_create(&lsnr->sndr_thread, NULL, sender_routine, &lsnr->sndr);
  if (pthread_res != 0)
    adptc_support_todo;

  lsnr->rcvr.user_cb = user_cb;
  lsnr->rcvr.user_ctx = user_ctx;
  pthread_res =
      pthread_create(&lsnr->rcvr_thread, NULL, receiver_routine, &lsnr->rcvr);
  if (pthread_res != 0)
    adptc_support_todo;
}

void adptc_listener_stop(adptc_listener const lhnd) {
  struct listener *const lsnr = lhnd;
  check_listener(lsnr);

  int pthread_res;

  // Indicate to the receiver thread that we are done.
  atomic_flag_clear(&lsnr->rcvr.continue_flag);

  // If the receiver thread is waiting on the condition variable,
  // wake it up so that it can respond to the request to stop.
  pthread_res = pthread_cond_signal(&lsnr->chan.rcvr_has_work);
  if (pthread_res != 0)
    adptc_support_todo;

  // Block until the receiver thread exits.
  pthread_res = pthread_join(lsnr->rcvr_thread, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  // Indicate to the sender thread that we are done.
  atomic_flag_clear(&lsnr->sndr.continue_flag);

  // Block until the sender thread exits.
  pthread_res = pthread_join(lsnr->sndr_thread, NULL);
  if (pthread_res != 0)
    adptc_support_todo;

  // Destroy the condition variable.
  pthread_res = pthread_cond_destroy(&lsnr->chan.rcvr_has_work);
  if (pthread_res != 0)
    adptc_support_todo;

  // Destroy the lock.
  pthread_res = pthread_mutex_destroy(&lsnr->chan.icmg_lock);
  if (pthread_res != 0)
    adptc_support_todo;
}

static void check_incoming(struct incoming *const icmg) { assert(icmg); }

[[nodiscard]] unsigned char const *
adptc_listener_get_incoming_data(adptc_listener_incoming const ihnd) {
  struct incoming *const icmg = ihnd;
  check_incoming(icmg);

  return icmg->data_buf;
}

[[nodiscard]] size_t
adptc_listener_get_incoming_data_len(adptc_listener_incoming const ihnd) {
  struct incoming *const icmg = ihnd;
  check_incoming(icmg);

  return icmg->data_len;
}

[[nodiscard]] double
adptc_listener_get_incoming_fill_ratio(adptc_listener_incoming const ihnd) {
  struct incoming *const icmg = ihnd;
  check_incoming(icmg);

  return (double)icmg->data_len / (double)LISTENER_BUF_SIZE;
}
