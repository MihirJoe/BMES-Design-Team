/// \file
/// \brief
/// Facilities for defining result types.

#ifndef ADAPT_CLIENT_RESULT_H
#define ADAPT_CLIENT_RESULT_H

#include <adapt/client/os.h>

#include <errno.h>

#define adptc_result_k_ok 0

#define adptc_result_kind_is_ok(result) (!(kind))
#define adptc_result_is_ok(result) (!(result).kind)

#define adptc_result_error_with_body(result_name, error_kind, ...)             \
  ((adptc_##result_name##_result){                                             \
      .kind = adptc_##result_name##_result_k_##error_kind##_error,             \
      .error_body = (__VA_ARGS__)})
#define adptc_result_error(result_name, error_kind)                            \
  ((adptc_##result_name##_result){                                             \
      .kind = adptc_##result_name##_result_k_##error_kind##_error})

#define adptc_result_os_error(result_name, error_kind, func)                   \
  adptc_result_error_with_body(                                                \
      result_name, error_kind,                                                 \
      (struct adptc_os_error){.func_name = #func, .code = errno})

#define adptc_result_ok_with_body(...)                                         \
  ((adptc_##result_name##_result){.kind = 0, .ok_body = (__VA_ARGS__)})
#define adptc_result_ok(result_name) ((adptc_##result_name##_result){.kind = 0})

#define ADPTC_RESULT_KIND(result_ident, ...)                                   \
  enum result_ident##_kind{result_ident##_k_ok, __VA_ARGS__};

#define ADPTC_RESULT_WITH_OK_AND_ERROR(result_ident, ok_type, error_type, ...) \
  ADPTC_RESULT_KIND(result_ident, __VA_ARGS__)                                 \
  typedef struct {                                                             \
    enum result_ident##_kind kind;                                             \
    ok_type ok_body;                                                           \
    error_type error_body;                                                     \
  } result_ident;

#define ADPTC_RESULT_WITH_OK(result_ident, ok_type, ...)                       \
  ADPTC_RESULT_KIND(result_ident, __VA_ARGS__)                                 \
  typedef struct {                                                             \
    enum result_ident##_kind kind;                                             \
    ok_type ok_body;                                                           \
  } result_ident;

#define ADPTC_RESULT_WITH_ERROR(result_ident, error_type, ...)                 \
  ADPTC_RESULT_KIND(result_ident, __VA_ARGS__)                                 \
  typedef struct {                                                             \
    enum result_ident##_kind kind;                                             \
    error_type error_body;                                                     \
  } result_ident;

#define ADPTC_RESULT_WITH_OS_ERROR(result_ident, ...)                          \
  ADPTC_RESULT_WITH_ERROR(result_ident, struct adptc_os_error, __VA_ARGS__)

#define ADPTC_RESULT(result_ident, ...)                                        \
  ADPTC_RESULT_KIND(result_ident, __VA_ARGS__)                                 \
  typedef struct {                                                             \
    enum result_ident##_kind kind;                                             \
  } result_ident;

#endif // ADAPT_CLIENT_RESULT_H
