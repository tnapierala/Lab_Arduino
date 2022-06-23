String sInput = ""; //variable for console user input
int inputToNumber = 0; //variable for converted input
const int LED1 = 7; //red led
int ledDelay = 0; //blink interval
void setup() {
 Serial.begin(9600);
 pinMode(LED1, OUTPUT); 
}
void loop() { 
 if (ledDelay > 0) {
 digitalWrite(LED1, HIGH);
 delay(ledDelay);
 digitalWrite(LED1, LOW);
 delay(ledDelay); 
 }
 if (Serial.available()) {
 sInput = "";
 while (Serial.available()) {
 delay(5);
 sInput += char( Serial.read() ); 
 }
 sInput.trim();
 inputToNumber = sInput.toInt(); 
 if (inputToNumber > 1) {
 if (inputToNumber % 2 == 0) {
 Serial.println("Blinking LED with 2Hz");
 ledDelay = 250; 
 } else { 
 Serial.println("Blinking LED with 4Hz");
 ledDelay = 125; 
 }
 } else { 
 Serial.println("Wrong input! Use only Integer numbers"); 
 } 
}
}
