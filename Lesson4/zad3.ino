void setup() {
  Serial.begin(9600);
}
void loop() {
  // reading value
  int sensorValue = analogRead(A0);
  // Convert the analog reading 
  //(which goes from 0 - 1023) to a voltage (0 - 5V)
  float vol = sensorValue * (5.0 / 1023.0);
  Serial.println(vol); // Print actual value voltage
}
