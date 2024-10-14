#include <HX711.h>
#include <MPU6050_light.h>
#include <RobojaxBTS7960.h>
#include <Wire.h>

#define SERIAL_BAUD 9600
#define SERIAL_TIMEOUT_MS 500

#define LOAD_CELL_DOUT_PIN 9
#define LOAD_CELL_SCK_PIN 10

#define LOAD_CELL_CALIBRATION_FACTOR                                           \
  -6500.f // -7050 worked for my 440lb max scale setup

#define LIN_ACT_RPWM_PIN 3 // output
#define LIN_ACT_R_EN_PIN 2 // input
#define LIN_ACT_R_IS_PIN 5 // output

#define LIN_ACT_LPWM_PIN 6 // output
#define LIN_ACT_L_EN_PIN 5 // input
#define LIN_ACT_L_IS_PIN 4 // output

#define LIN_ACT_MIN_MOVE_DURATION_MS 50
#define LIN_ACT_MAX_MOVE_DURATION_MS 10700

static HX711 LoadCell;
static MPU6050 IMU(Wire);
static RobojaxBTS7960 LinAct(LIN_ACT_R_EN_PIN, LIN_ACT_RPWM_PIN,
                             LIN_ACT_R_IS_PIN, LIN_ACT_L_EN_PIN,
                             LIN_ACT_LPWM_PIN, LIN_ACT_L_IS_PIN, 1);

static unsigned long LinActMotionStartTime = 0;
static unsigned int LinActMotionDurationMs = 0;

void setupSerial() {
  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(SERIAL_TIMEOUT_MS);
}

void setupLoadCell() {
  LoadCell.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  LoadCell.set_scale();
  LoadCell.tare(); // Reset the scale to 0
  // Serial.println("Load cell initialized");

  LoadCell.read_average(); // Get a baseline reading
}

void setupIMU() {
  Wire.begin();

  byte status = IMU.begin();
  // Serial.print(F("MPU6050 status: "));
  // Serial.println(status);
  // while (status != 0) {
  // } // stop everything if could not connect to MPU6050

  IMU.calcOffsets(); // gyro and accelerometer
  delay(1000);
}

void setupLinearActuator() { LinAct.begin(); }

void setup() {
  setupSerial();
  setupLoadCell();
  setupIMU();
  setupLinearActuator();

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

class CommandArgs {
  String S;

public:
  CommandArgs(String S) : S{S} {}

  bool hasNext() const { return this->S.length() > 0; }

  String next() {
    String Next;

    int const CommaIdx = this->S.indexOf(',');
    if (CommaIdx == -1) {
      Next = this->S;
      this->S = String();
    } else {
      Next = this->S.substring(0, CommaIdx);
      this->S = this->S.substring(CommaIdx + 1);
    }

    Next.trim();
    return Next;
  }
};

struct Angles {
  float angleX;
  float angleY;
  float angleZ;
};

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

void serveMeasureCommand(class CommandArgs &Args) {
  float const force = LoadCell.get_units(); // get force
  Angles const avgAngles = measureAverageAngles();

  Serial.print(
      avgAngles.angleX); // update based on calibration factor and calcuation
  Serial.print(",");
  Serial.print(force);
  Serial.println();
}

enum LinearActuatorDirection {
  LAD_Retract = 0,
  LAD_Extend = 1,
};

void moveLinearActuator(unsigned int const DurationMs,
                        int const Dir) {
  LinAct.rotate(100, Dir);
  LinActMotionStartTime = millis();
  LinActMotionDurationMs = DurationMs;
}

void serveMotionCommand(class CommandArgs &Args, int const Dir) {
  if (!Args.hasNext())
    return;

  unsigned int DurationMs = Args.next().toInt();
  DurationMs =
      constrain(DurationMs, LIN_ACT_MIN_MOVE_DURATION_MS,
                LIN_ACT_MAX_MOVE_DURATION_MS);

  moveLinearActuator(DurationMs, Dir);
}

void serveExtendCommand(class CommandArgs &Args) {
  serveMotionCommand(Args, LAD_Extend);
}

void serveRetractCommand(class CommandArgs &Args) {
  serveMotionCommand(Args, LAD_Retract);
}

void stopLinearActuator() {
  LinAct.stop();
  LinActMotionDurationMs = 0;
}

void serveStopCommand(class CommandArgs &Args) { stopLinearActuator(); }

void serveCommand(String const &Name, class CommandArgs &Args) {
  if (Name == "m")
    serveMeasureCommand(Args);
  else if (Name == "e")
    serveExtendCommand(Args);
  else if (Name == "r")
    serveRetractCommand(Args);
  else if (Name == "s")
    serveStopCommand(Args);
}

void serveIncomingCommand() {
  String const Cmd = Serial.readStringUntil('\n');
  if (!Cmd)
    return;

  CommandArgs Args(Cmd);
  if (!Args.hasNext())
    return;

  String const Name = Args.next();
  serveCommand(Name, Args);
}

void loop() {
  IMU.update();
  LoadCell.set_scale(
      LOAD_CELL_CALIBRATION_FACTOR); // Adjust to this calibration factor for
                                     // force

  if ((LinActMotionDurationMs > 0) &&
      ((millis() - LinActMotionStartTime) > LinActMotionDurationMs))
    stopLinearActuator();

  if (Serial.available() > 0)
    serveIncomingCommand();
}
