#ifndef ADAPT_SKETCH_LINEARACTUATOR_HPP
#define ADAPT_SKETCH_LINEARACTUATOR_HPP

#include "config.h"

namespace as {

/// \brief The linear actuator.
class LinearActuator {
private:
  enum class Direction {
    In = 0,
    Out = 1,
  };

  /// \brief The value of millis when the current motion started.
  ///
  /// This value is used to determine motion completion.
  ///
  /// If we are not currently moving
  /// (i.e., MotionDurationMs is 0),
  /// this value is not defined.
  unsigned long MotionStartTime{0};
  unsigned long MotionDurationMs{0};

  void configurePins() {
    pinMode(ADAPT_LIN_ACT_L_EN_PIN, OUTPUT);
    pinMode(ADAPT_LIN_ACT_R_EN_PIN, OUTPUT);

    pinMode(ADAPT_LIN_ACT_LPWM_PIN, OUTPUT);
    pinMode(ADAPT_LIN_ACT_RPWM_PIN, OUTPUT);
  }

  bool motionIsComplete() const {
    return millis() - this->MotionStartTime > this->MotionDurationMs;
  }

  void move(unsigned long const DurationMs, Direction const Dir) {
    static int const ActivePwmPinMap[2] = {/* In */
                                           ADAPT_LIN_ACT_LPWM_PIN,
                                           /* Out */
                                           ADAPT_LIN_ACT_RPWM_PIN};

    int const ActivePwmPin = ActivePwmPinMap[static_cast<int>(Dir)];
    int const InactivePwmPin = ActivePwmPinMap[!static_cast<int>(Dir)];

    digitalWrite(InactivePwmPin, LOW);
    digitalWrite(ActivePwmPin, HIGH);

    digitalWrite(ADAPT_LIN_ACT_L_EN_PIN, HIGH);
    digitalWrite(ADAPT_LIN_ACT_R_EN_PIN, HIGH);

    this->MotionStartTime = millis();
    this->MotionDurationMs = DurationMs;
  }

public:
  void begin() { this->configurePins(); }

  bool isMoving() const { return this->MotionDurationMs > 0; }

  void update() {
    if (this->isMoving() && this->motionIsComplete())
      this->stop();
  }

  void extend(unsigned long const DurationMs) {
    this->move(DurationMs, Direction::Out);
  }

  void retract(unsigned long const DurationMs) {
    this->move(DurationMs, Direction::In);
  }

  void home() { this->retract(ADAPT_LIN_ACT_STROKE_DURATION_MS); }

  void stop() {
    // Inhibit the half-bridges.
    digitalWrite(ADAPT_LIN_ACT_L_EN_PIN, LOW);
    digitalWrite(ADAPT_LIN_ACT_R_EN_PIN, LOW);

    // Zero the input to the half-bridges, to be safe.
    digitalWrite(ADAPT_LIN_ACT_LPWM_PIN, LOW);
    digitalWrite(ADAPT_LIN_ACT_RPWM_PIN, LOW);

    // End the motion.
    this->MotionDurationMs = 0;
  }
};

} // namespace as

#endif // ADAPT_SKETCH_LINEARACTUATOR_HPP
