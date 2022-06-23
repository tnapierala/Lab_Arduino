String sInput = ""; // variable for console
const int LED1 = 6; // yellow led
const int LED2 = 5; // red led 
void setup() {
 Serial.begin(9600);
 pinMode(LED1, OUTPUT);
 pinMode(LED2, OUTPUT); 
}
void loop() {
 if (Serial.available()) { sInput = "";
 while (Serial.available()) {
 delay(5);
 sInput += char(Serial.read());
 sInput.toLowerCase(); // change input to lower case
 } 
 if (sInput == "a") {
 digitalWrite(LED1, HIGH);
 digitalWrite(LED2, LOW);
 Serial.println("Turning only Yellow LED ON");
 } else if (sInput == "b") {
 digitalWrite(LED1, HIGH);
 digitalWrite(LED2, HIGH);
 Serial.println("Turning Yellow&Red LED's ON");
 } else { 
 Serial.println("Wrong command! Use 'a' or 'b'");
 } 
 }
}
