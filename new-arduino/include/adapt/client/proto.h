/// \file
/// \brief
/// Client-oriented ADAPT protocol facilities.

#ifndef ADAPT_CLIENT_PROTO_H
#define ADAPT_CLIENT_PROTO_H

#include <adapt/client/result.h>

/// \class adptc_proto_response_decoder
typedef void *adptc_proto_response_decoder;

union adptc_proto_response_body {
  struct {
    float value;
  } float_measurement;
};

struct adptc_proto_response {
  unsigned char status_code;
  union adptc_proto_response_body body;
};

/// \cond
ADPTC_RESULT_WITH_OS_ERROR(adptc_proto_send_request_result,
                           adptc_proto_send_request_result_k_write_error)

ADPTC_RESULT_WITH_OK_TYPE_AND_OS_ERROR(
    adptc_proto_response_decoder_create_result, adptc_proto_response_decoder,
    adptc_proto_response_decoder_create_result_k_alloc_decoder_error)
/// \endcond

[[nodiscard]] char const *adptc_proto_status_code_to_str(unsigned char code);

[[nodiscard]] unsigned char adptc_proto_build_request(unsigned char code,
                                                      unsigned char body);

adptc_proto_send_request_result adptc_proto_try_send_request(int fd,
                                                             unsigned char req);

[[nodiscard]] float adptc_proto_unmarshall_float(unsigned long marsh);

/// \public \static \memberof adptc_proto_response_decoder
/// \brief
/// Attempts to create a new response decoder.
adptc_proto_response_decoder_create_result
adptc_proto_response_decoder_try_create(void);

/// \public \memberof adptc_proto_response_decoder
/// \brief
/// Destroys a response decoder.
///
/// This method releases resources owned by \a decoder.
///
/// On return from this method,
/// \a decoder no longer exists
/// and must not be used in further method calls.
void adptc_proto_response_decoder_destroy(adptc_proto_response_decoder decoder);

/// \public \memberof adptc_proto_response_decoder
/// \brief
/// Feeds the decoder one incoming byte.
///
/// If \a byte is determined to terminate a response,
/// a pointer to the decoded response is returned.
///
/// \param[in] byte
/// The incoming byte in question.
/// \return
/// A pointer to the decoded response,
/// or \c NULL if \a byte does not terminate a response.
struct adptc_proto_response const *
adptc_proto_response_decoder_feed(adptc_proto_response_decoder decoder,
                                  unsigned char byte);

#endif // ADAPT_CLIENT_PROTO_H
