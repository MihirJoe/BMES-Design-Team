#include "HX711.h"
#include "Wire.h"
#include <MPU6050_light.h>

// Load Cell Initialization

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

HX711 scale;

float calibration_factor = -6500; // -6500 //-7050 worked for my 440lb max scale setup
char userInput;

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
    
    userInput = Serial.read();

    scale.set_scale(calibration_factor); //Adjust to this calibration factor for force

    if (userInput == 'g') {
      scale.set_scale(calibration_factor); //Adjust to this calibration factor for force
      measureAngles = true;
      // timer = millis();
    }

    if (measureAngles) {
      force = scale.get_units(); // get force

      Angles avgAngles = measureAverageAngles();
      measureAngles = false;

      Serial.print(avgAngles.angleX); // update based on calibration factor and calcuation
      Serial.print(",");
      Serial.print(force);
    }
  }
}

