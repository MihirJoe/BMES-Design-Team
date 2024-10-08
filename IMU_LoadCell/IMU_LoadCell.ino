#include <HX711.h>
#include <MPU6050_light.h>
#include <RobojaxBTS7960.h>
#include <Wire.h>

#define RPWM_1 3 // RPWM pin (output)
#define R_EN_1 2 // R_EN pin (input)
#define R_IS_1 5 // R_IS pin (output)

#define LPWM_1 6 // LPWM pin (output)
#define L_EN_1 5 // L_EN pin (input)
#define L_IS_1 4 // L_IS pin (output)

RobojaxBTS7960 motor1(R_EN_1, RPWM_1, R_IS_1, L_EN_1, LPWM_1, L_IS_1, 1); // define motor 1 object

// Load Cell Initialization

#define LOADCELL_DOUT_PIN  9
#define LOADCELL_SCK_PIN  10

HX711 scale;

float calibration_factor = -6500; // -6500 //-7050 worked for my 440lb max scale setup

// IMU Initialization

MPU6050 mpu(Wire);
unsigned long timer = 0;
bool measureAngles = false;
float sumX = 0, sumY = 0, sumZ = 0, force = 0;
int readingCount = 0;

struct Angles {
  float angleX;
  float angleY;
  float angleZ;
};

void setup() {
  Serial.begin(9600);

  motor1.begin();

  // Load Cell Calibration
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  // Serial.println("Load cell initialized");

  long zero_factor = scale.read_average(); //Get a baseline reading

  // IMU Calibration
  Wire.begin();

  byte status = mpu.begin();
  // Serial.print(F("MPU6050 status: "));
  // Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050

  mpu.calcOffsets(); // gyro and accelerometer
  delay(1000);
  Serial.print("a");
  // Serial.flush();
  // TODO: send status updates to GUI on calibration, initial position

  // TODO: account for initial position?

  // Serial.println("Initial:");
  // Serial.print("X : ");
  // Serial.print(mpu.getAngleX());
  // Serial.print("\tY : ");
  // Serial.print(mpu.getAngleY());
  // Serial.print("\tZ : ");
  // Serial.println(mpu.getAngleZ());
  // Serial.println();


}

Angles measureAverageAngles() {
  // sumX = 0;
  // sumY = 0;
  // sumZ = 0;
  // readingCount = 0;

  // while ((millis() - timer) < 10) { // Collect data for 1 second
  //   mpu.update();
  //   sumX += mpu.getAngleX();
  //   sumY += mpu.getAngleY();
  //   sumZ += mpu.getAngleZ();
  //   readingCount++;
  // }
  mpu.update();
  Angles result;
  result.angleX = mpu.getAngleX();
  result.angleY = mpu.getAngleY();
  result.angleZ = mpu.getAngleZ();
  // if (readingCount > 0) {

  // } else {
  //   result.angleX = 0;
  //   result.angleY = 0;
  //   result.angleZ = 0;
  // }

  return result;
}

void loop() {
  mpu.update(); // update IMU

  if(Serial.available()>0)
  {
    scale.set_scale(calibration_factor); //Adjust to this calibration factor for force

    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any extra whitespace or newlines

    if (command.startsWith("rotate")) {
      int firstComma = command.indexOf(',');
      int secondComma = command.indexOf(',', firstComma + 1);

      int speed = command.substring(firstComma + 1, secondComma).toInt();
      int direction = command.substring(secondComma + 1).toInt();

      motor1.rotate(speed, direction == 1 ? CW : CCW);
    } else if (command == "stop") {
      motor1.stop();
    } else if (command == "getforce") {
      force = scale.get_units(); // get force

      Angles avgAngles = measureAverageAngles();
      measureAngles = false;

      Serial.print(avgAngles.angleX); // update based on calibration factor and calcuation
      Serial.print(",");
      Serial.print(force);
    }
  }
}
