/// \file
/// Magic numbers.

#ifndef ADAPT_SKETCH_CONFIG_H
#define ADAPT_SKETCH_CONFIG_H

#define ADPTS_HX711_DOUT_PIN 9
#define ADPTS_HX711_SCK_PIN 10

#define ADPTS_HX711_SCALE -7770.f

#define ADPTS_HX711_TIMEOUT_MS 10

#define ADPTS_I2C_TIMEOUT_MS 10

/// \brief
/// The INH ("inhibit") pin on the right BTS7960 (half-bridge).
///
/// INH is active low.
#define ADPTS_LIN_ACT_R_EN_PIN 2
/// \brief
/// The IN ("input") pin on the right BTS7960 (half-bridge).
#define ADPTS_LIN_ACT_RPWM_PIN 3

/// \brief
/// The INH ("inhibit") pin on the left BTS7960 (half-bridge).
///
/// INH is active low.
#define ADPTS_LIN_ACT_L_EN_PIN 5
/// \brief
/// The IN ("input") pin on the left BTS7960 (half-bridge).
#define ADPTS_LIN_ACT_LPWM_PIN 6

/// \brief
/// The duration, in milliseconds,
/// of the linear actuator stroke.
#define ADPTS_LIN_ACT_STROKE_DURATION_MS 10700

#endif // ADAPT_SKETCH_CONFIG_H
