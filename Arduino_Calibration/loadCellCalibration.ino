#include <HX711.h>

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

HX711 scale;

float calibration_factor = -1229; // Initial calibration factor
float target_weight = 0.0; // Target weight in lbs
float tolerance = 0.01; // Tolerance for calibration in lbs
unsigned long start_time = 0; // Variable to store start time for averaging
unsigned int num_readings = 0; // Number of readings for averaging
float sum_readings = 0.0; // Sum of readings for averaging
boolean calibrated = false; // Flag to indicate if calibration is done

void setup() {
  Serial.begin(9600);
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
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 7);
  Serial.print(" lbs, Calibration factor: ");
  Serial.println(calibration_factor);

  if (!calibrated) {
    // Calibration process
    float measured_weight = scale.get_units();
    float diff = measured_weight - target_weight;

    if (abs(diff) <= tolerance) {
      calibrated = true;
      Serial.println("Calibration done!");
    } else {
      calibration_factor += diff * 0.1; // Adjust calibration factor
    }
  }


  delay(1000);
}

# add use average of previous 5 seconds
# share calibration to the python GUI
# Add logic to change direction of calibration factor if going wrong way