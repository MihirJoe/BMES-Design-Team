#ifndef ADAPT_CONFIG_H
#define ADAPT_CONFIG_H

#define ADAPT_HX711_DOUT_PIN 9
#define ADAPT_HX711_SCK_PIN 10

#define ADAPT_HX711_SCALE -7770.f

#define ADAPT_HX711_TIMEOUT 10

/// \brief The INH ("inhibit") pin on the right BTS7960 (half-bridge).
///
/// INH is active low.
#define ADAPT_LIN_ACT_R_EN_PIN 2
/// \brief The IN ("input") pin on the right BTS7960 (half-bridge).
#define ADAPT_LIN_ACT_RPWM_PIN 3

/// \brief The INH ("inhibit") pin on the left BTS7960 (half-bridge).
///
/// INH is active low.
#define ADAPT_LIN_ACT_L_EN_PIN 5
/// \brief The IN ("input") pin on the left BTS7960 (half-bridge).
#define ADAPT_LIN_ACT_LPWM_PIN 6

/// \brief The duration, in milliseconds,
///        of the linear actuator stroke.
#define ADAPT_LIN_ACT_STROKE_DURATION_MS 10700

#endif // ADAPT_CONFIG_H
