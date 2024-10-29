#include <HX711.h>
#include <MPU6050_light.h>
#include <Wire.h>

#define SERIAL_BAUD 9600
#define SERIAL_TIMEOUT_MS 500

#define COMMAND_BUF_LEN 128

#define SCALE_DOUT_PIN 9
#define SCALE_SCK_PIN 10

#define SCALE_CALIBRATION_FACTOR -7770.f

#define LIN_ACT_R_EN_PIN 2
#define LIN_ACT_RPWM_PIN 3

#define LIN_ACT_L_EN_PIN 5
#define LIN_ACT_LPWM_PIN 6

#define LIN_ACT_MIN_MOVE_DURATION_MS 50
#define LIN_ACT_MAX_MOVE_DURATION_MS 10700

/// \brief The HX711 scale.
HX711 Scale;
/// \brief The MPU6050 IMU.
MPU6050 IMU(Wire);

/// \brief The linear actuator.
class LinearActuator {
public:
  enum Direction {
    In,
    Out,
  };

  void begin() { this->configurePins(); }

  bool isMoving() const { return this->MotionDurationMs > 0; }

  void update() {
    if (this->isMoving() &&
        ((millis() - this->MotionStartTime) > this->MotionDurationMs))
      this->stop();
  }

  void move(unsigned int const DurationMs, Direction const Dir) {
    this->actuate(Dir);
    this->MotionStartTime = millis();
    this->MotionDurationMs = DurationMs;
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

    // Set the PWM pins to 0% duty cycle for good measure.
    digitalWrite(LIN_ACT_RPWM_PIN, LOW);
    digitalWrite(LIN_ACT_LPWM_PIN, LOW);

    this->MotionDurationMs = 0;
  }

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
};

/// \brief The linear actuator.
LinearActuator LinAct;

struct Angles {
  float angleX;
  float angleY;
  float angleZ;
};

void setupLinearActuator() {
  LinAct.begin();
  LinAct.home();
}

void setupScale() {
  Scale.begin(SCALE_DOUT_PIN, SCALE_SCK_PIN);
  Scale.set_scale();
  Scale.tare(); // Reset the scale to 0
  // Serial.println("Load cell initialized");

  Scale.read_average(); // Get a baseline reading

  Scale.set_scale(SCALE_CALIBRATION_FACTOR); // Adjust to this calibration
                                             // factor for force
}

void setupIMU() {
  Wire.begin();

  byte status = IMU.begin();
  // Serial.print(F("MPU6050 status: "));
  // Serial.println(status);
  while (status != 0) {
  } // stop everything if could not connect to MPU6050

  IMU.calcOffsets(); // gyro and accelerometer
  // Serial.println("Calculating offsets complete.");
}

void setupSerial() {
  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(SERIAL_TIMEOUT_MS);
}

void setup() {
  setupLinearActuator();
  setupScale();
  setupIMU();
  setupSerial();

  delay(1000);
  Serial.print("a");
  // Serial.flush();
  // TODO: send status updates to GUI on calibration, initial position

  // TODO: account for initial position?

  // Serial.println("Initial:");
  // Serial.print("X : ");
  // Serial.print(IMU.getAngleX());
  // Serial.print("\tY : ");
  // Serial.print(IMU.getAngleY());
  // Serial.print("\tZ : ");
  // Serial.println(IMU.getAngleZ());
  // Serial.println();
}

struct Angles measureAverageAngles() {
  // sumX = 0;
  // sumY = 0;
  // sumZ = 0;
  // readingCount = 0;

  // while ((millis() - timer) < 10) { // Collect data for 1 second
  //   IMU.update();
  //   sumX += IMU.getAngleX();
  //   sumY += IMU.getAngleY();
  //   sumZ += IMU.getAngleZ();
  //   readingCount++;
  // }
  IMU.update();
  Angles result;
  result.angleX = IMU.getAngleX();
  result.angleY = IMU.getAngleY();
  result.angleZ = IMU.getAngleZ();
  // if (readingCount > 0) {

  // } else {
  //   result.angleX = 0;
  //   result.angleY = 0;
  //   result.angleZ = 0;
  // }

  return result;
}

void serveMeasureCommand(char const *const Args) {
  float const force = Scale.get_units(); // get force
  Angles const avgAngles = measureAverageAngles();

  Serial.print(
      avgAngles.angleX); // update based on calibration factor and calcuation
  Serial.print(",");
  Serial.print(force);
  Serial.println();
}

void serveMotionCommand(char const *const Args,
                        LinearActuator::Direction const Dir) {
  unsigned int DurationMs;
  if (sscanf(Args, "%u", &DurationMs) != 1)
    return;

  DurationMs = constrain(DurationMs, LIN_ACT_MIN_MOVE_DURATION_MS,
                         LIN_ACT_MAX_MOVE_DURATION_MS);
  LinAct.move(DurationMs, Dir);
}

void serveExtendCommand(char const *const Args) {
  serveMotionCommand(Args, LinearActuator::Direction::Out);
}

void serveRetractCommand(char const *const Args) {
  serveMotionCommand(Args, LinearActuator::Direction::In);
}

void serveStopCommand(char const *const Args) { LinAct.stop(); }

void serveCommand(char const Name, char const *const Args) {
  switch (Name) {
  case 'g':
    serveMeasureCommand(Args);
    break;
  case 'e':
    serveExtendCommand(Args);
    break;
  case 'r':
    serveRetractCommand(Args);
    break;
  case 's':
    serveStopCommand(Args);
    break;
  }
}

void serveIncomingCommand() {
  static char CmdBuf[COMMAND_BUF_LEN];

  size_t const BytesRead =
      Serial.readBytesUntil('\n', CmdBuf, COMMAND_BUF_LEN - 1);
  CmdBuf[BytesRead] = 0;

  char Name;
  size_t ArgsOff;
  if (sscanf(CmdBuf, " %c%zn", &Name, &ArgsOff) != 1)
    return;

  char const *const Args = CmdBuf + ArgsOff;
  serveCommand(Name, Args);
}

void loop() {
  IMU.update();
  LinAct.update();

  if (Serial.available() > 0)
    serveIncomingCommand();
}
