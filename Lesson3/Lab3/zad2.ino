String sInput = ""; //variable for console user input
int LED1 = 7; //green led
int LED2 = 6; // yellow led
int LED3 = 5; // red led 
void setup() { Serial.begin(9600);
 pinMode(LED1, OUTPUT);
 pinMode(LED2, OUTPUT);
 pinMode(LED3, OUTPUT); 
}
 
void loop() { 
 if (Serial.available()) {
 sInput = "";
 while (Serial.available()) {
 delay(5); 
 sInput += char(Serial.read());
 sInput.toLowerCase(); //change user input to lower case 
 } 
 if (sInput == "dioda") {
 digitalWrite(LED1, HIGH);
 digitalWrite(LED2, LOW);
 digitalWrite(LED3, LOW); 
 Serial.println("Turning Green LED ON");
 } else if (sInput == "swiatla") {
 digitalWrite(LED1, HIGH);
 digitalWrite(LED2, HIGH);
 digitalWrite(LED3, HIGH); 
 Serial.println("Turning Yellow&Green&Red LED's ON"); 
 } else { 
 Serial.println("Wrong command! Use only 'dioda' or 'swiatla'"); 
 } 
 } 
}