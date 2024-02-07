#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  clearEEPROM();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}


void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);  // Write 0 to each EEPROM address
  }
}
