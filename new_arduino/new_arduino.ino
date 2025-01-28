// As of writing (Jan 21),
// `arduino_main.ino` is not in a functional state.
// There are spontaneous and unpredictable issues
// that manifest when connected to the Python script (GUI),
// notably the Arduino "freezing"
// within a short time (<1 minute) of starting data collection.
//
// We have decided to rewrite that program one component at a time
// to try and isolate the issue.
// The current theory is there is a problem with our serial protocol
// related to the rate of data transmission.
// One action we have taken to reduce the amount of serial communications
// is to replace the singular "measure" command
// (which must be sent for each and every data point)
// with "start measuring" and "stop measuring" commands
// which instruct the Arduino to continuously output data points
// until told to stop.
//
// A good next step
// would be to replace the current ASCII-based protocol
// with a binary one.
// For example, in the old Arduino program,
// the "measure" command collects the data point,
// renders it as an ASCII string,
// and outputs that to serial.
// I propose we save some bits
// and just transmit the raw IMU and HX711 data
// to later be decoded by the Python script.

#include <HX711.h>

#define SERIAL_BAUD 115200

#define SCALE_DOUT_PIN 9
#define SCALE_SCK_PIN 10

#define SCALE_CALIBRATION_FACTOR -7770.f

#define LIN_ACT_R_EN_PIN 2
#define LIN_ACT_RPWM_PIN 3

#define LIN_ACT_L_EN_PIN 5
#define LIN_ACT_LPWM_PIN 6

#define LIN_ACT_MAX_MOVE_DURATION_MS 10700

/// \brief The linear actuator.
class LinearActuator {
public:
  enum class Direction {
    In = 0,
    Out = 1,
  };

private:
  unsigned long MotionStartTime{0};
  unsigned int MotionDurationMs{0};

  void configurePins() {
    pinMode(LIN_ACT_R_EN_PIN, OUTPUT);
    pinMode(LIN_ACT_RPWM_PIN, OUTPUT);

    pinMode(LIN_ACT_L_EN_PIN, OUTPUT);
    pinMode(LIN_ACT_LPWM_PIN, OUTPUT);
  }

  void actuate(Direction const Dir) {
    int ActivePwmPin;
    int InactivePwmPin;

    switch (Dir) {
    case Direction::In:
      ActivePwmPin = LIN_ACT_LPWM_PIN;
      InactivePwmPin = LIN_ACT_RPWM_PIN;
      break;
    case Direction::Out:
      ActivePwmPin = LIN_ACT_RPWM_PIN;
      InactivePwmPin = LIN_ACT_LPWM_PIN;
      break;
    }

    analogWrite(ActivePwmPin, 0xff);
    digitalWrite(InactivePwmPin, LOW);

    // Enable the motor.
    digitalWrite(LIN_ACT_R_EN_PIN, HIGH);
    digitalWrite(LIN_ACT_L_EN_PIN, HIGH);
  }

public:
  void begin() { this->configurePins(); }

  bool isMoving() const { return this->MotionDurationMs > 0; }

  void update() {
    if (this->isMoving() &&
        (millis() - this->MotionStartTime > this->MotionDurationMs))
      this->stop();
  }

  void move(unsigned int const DurationMs, Direction const Dir) {
    this->actuate(Dir);
    this->MotionStartTime = millis();
    this->MotionDurationMs = DurationMs;
  }

  void extend(unsigned long const DurationMs) {
    this->move(DurationMs, Direction::Out);
  }

  void retract(unsigned long const DurationMs) {
    this->move(DurationMs, Direction::In);
  }

  void home() {
    this->actuate(Direction::In);
    delay(LIN_ACT_MAX_MOVE_DURATION_MS);
    this->stop();
  }

  void stop() {
    // Disable the motor.
    digitalWrite(LIN_ACT_R_EN_PIN, LOW);
    digitalWrite(LIN_ACT_L_EN_PIN, LOW);

    // Disable the PWM pins.
    digitalWrite(LIN_ACT_RPWM_PIN, LOW);
    digitalWrite(LIN_ACT_LPWM_PIN, LOW);

    this->MotionDurationMs = 0;
  }
};

/// \brief The HX711 scale.
static HX711 Scale;
/// \brief The linear actuator.
static LinearActuator LinAct;

static bool Measuring = false;

static void setupSerial() { Serial.begin(SERIAL_BAUD); }

static void setupLinearActuator() {
  LinAct.begin();
  LinAct.home();
}

static void setupScale() {
  Scale.begin(SCALE_DOUT_PIN, SCALE_SCK_PIN);
  Scale.set_scale();
  Scale.tare();                              // Reset the scale to 0
  Scale.read_average();                      // Get a baseline reading
  Scale.set_scale(SCALE_CALIBRATION_FACTOR); // Adjust to this calibration
                                             // factor for force
}

void setup() {
  setupSerial();
  setupLinearActuator();
  setupScale();
}

static void measure() {
  float const force = Scale.get_units(); // get force
  // Angles const avgAngles = measureAverageAngles();

  // Serial.print(
  //     avgAngles.angleX); // update based on calibration factor and calcuation
  // Serial.print(',');
  Serial.print(force);
  Serial.println();
}

static void handleCommand(int ch) {
  switch (ch) {
  case 'M':
    Measuring = false;
    break;
  case 'm':
    Measuring = true;
    break;
  case 'e':
    LinAct.extend(200);
    break;
  case 'r':
    LinAct.retract(200);
    break;
  }
}

void loop() {
  LinAct.update();

  if (Measuring)
    measure();

  int ch = Serial.read();
  if (ch >= 0)
    handleCommand(ch);
}
