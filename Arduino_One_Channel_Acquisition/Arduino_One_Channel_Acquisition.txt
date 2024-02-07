

//float t;
float v;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop()
{
  Serial.print(analogRead(A0)/204.6,2);
  Serial.print(' ');
  delay(0.25);
}
