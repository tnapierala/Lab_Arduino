String sInput = ""; // variable for console user input
String interval = ""; // variable for interval input string
int intervalNumber = 0; // variable for interval input converted to number
const int Led1 = 13; // green led 
const int Led2 = 12; // yellow led
const int Led3 = 11; // red led 
int ledDelay = 0; // blink interval 
void setup() {
 Serial.begin(9600);
 pinMode(Led1, OUTPUT);
 pinMode(Led2, OUTPUT);
 pinMode(Led3, OUTPUT);
 Serial.println("Use command 'swiatla' or 'dioda <blink interval>'"); 
 Serial.println("Max interval for LED blink is: 999ms"); 
}
void trafficLights() { 
 // function for traffic lights
 ledDelay = 0;
 digitalWrite(Led3, HIGH);
 delay(4000);
 digitalWrite(Led2, HIGH);
 delay(1500);
 digitalWrite(Led3, LOW); digitalWrite(Led2, LOW);
 digitalWrite(Led1, HIGH);
 delay(4000);
 digitalWrite(Led1, LOW); digitalWrite(Led2, HIGH);
 delay(2200); digitalWrite(Led2, LOW);
 digitalWrite(Led3, HIGH); 
} 
void loop() {
 if (ledDelay > 0) {
 digitalWrite(Led1, HIGH); delay(ledDelay);
 digitalWrite(Led1, LOW); delay(ledDelay); 
 }
 if (Serial.available()) {
 sInput = "";
 while (Serial.available()) {
 delay(5); 
 sInput += char(Serial.read()); 
 sInput.toLowerCase(); // change user input to lower case 
 } 
 if (sInput.substring(0, 5) == "dioda") {
 digitalWrite(Led3, LOW);
 interval = sInput.substring(6, 9);
 interval.trim();
 intervalNumber = interval.toInt();
 if (intervalNumber > 1) {
 ledDelay = intervalNumber; 
 Serial.println("\n Blinking Green LED with provided interval: "); 
 Serial.println(ledDelay); 
 } else { 
 Serial.println("Wrong input! Use only Integer numbers"); 
 } 
 } else if (sInput == "swiatla") {
 Serial.println("Traffic lights");
 digitalWrite(Led1, LOW);
 trafficLights(); 
 } else { 
 Serial.println("Wrong command! Use 'dioda' or 'swiatla'"); 
 } 
 }
}