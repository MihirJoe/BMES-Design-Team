void setup() {
  // put your setup code here, to run once:
  //Options for baud rate: 300, 600, 1200, 2400, 4800, 9600
  //, 14400, 19200, 28800, 38400, 57600, or 115200
  Serial.begin(115200);
  delay(100); 
  }

void loop()
{
  //put your main code here, to run repeatedly:
  //Serial.println(micros());
  //Outputs the analog read from the sensor
  Serial.print(analogRead(A0)/204.6,2);
  Serial.print(' ');
  delay(0.25); //change to 0.5
  
  Serial.print(analogRead(A5)/204.6,2);
  Serial.print(' ');
  delay(0.25); //change to 0.5
  }
