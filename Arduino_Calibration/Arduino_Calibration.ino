void setup() {

//***Always select Tools > Fix Encoding and Reload
Serial.begin(115200);
delay(1000);
}


#define MAX 5000
void loop() {
  for (int i=0; i< MAX; i++){
  Serial.print(sin(map(i,0,MAX,0,628)/100.0)+2.5,2);
  Serial.print(' ');
  delay(0.25);
  }
}
