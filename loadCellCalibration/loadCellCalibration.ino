#include <HX711.h>
#include <SoftwareSerial.h>

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

HX711 scale;

float calibration_factor = -6500; // best for wood
float target_weight = 0.0; // Target weight in lbs
float tolerance = 0.01; // Tolerance for calibration in lbs
unsigned long start_time = 0; // Variable to store start time for averaging
unsigned int num_readings = 0; // Number of readings for averaging
float sum_readings = 0.0; // Sum of readings for averaging
boolean calibrated = false; // Flag to indicate if calibration is done
float avg_weight = 0.0; // Average weight of the previous 5 seconds

SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // Reset the scale to 0

  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  // Start the calibration timer
  start_time = millis();
}

void loop() {
  // Always print the current reading and calibration factor
  scale.set_scale(calibration_factor);
  float current_weight = scale.get_units();
  Serial.print("Reading: ");
  Serial.print(current_weight, 7);
  Serial.print(" lbs, Calibration factor: ");
  Serial.println(calibration_factor);

  // Calculate average weight of the previous 5 seconds
  unsigned long current_time = millis();
  if (current_time - start_time >= 5000) {
    avg_weight = sum_readings / num_readings;
    sum_readings = 0.0;
    num_readings = 0;
    start_time = current_time;
  } else {
    sum_readings += current_weight;
    num_readings++;
  }

  if (!calibrated) {
    // Calibration process
    float diff = avg_weight - target_weight;

    if (abs(diff) <= tolerance) {
      calibrated = true;
      Serial.println("Calibration done!");
      mySerial.println("Calibration:" + String(calibration_factor));
    } else {
      // Adjust calibration factor
      float new_calibration_factor = calibration_factor + diff * 0.1;
      float new_avg_weight = sum_readings / num_readings;
      float new_diff = new_avg_weight - target_weight;

      if (abs(new_diff) > abs(diff)) {
        // Revert to the previous calibration factor if the new one worsens the tolerance
        calibration_factor -= diff * 0.1;
      } else {
        calibration_factor = new_calibration_factor;
      }
    }
  }

  delay(1000);
}
