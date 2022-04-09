int buttonState = 0;
int bCounter = 0;
const int buttPin = 2;
const int ledPin =  12;
int ledState = LOW;	// ledState used to set the LED

unsigned long previousMillis = 0;	 // will store last time LED was updated
const long intervalSlow = 1000;	// interval at which to blink
const long intervalFast = 500;	// interval at which to blink

void setup() {
  Serial.begin(9600);
  pinMode(buttPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void slow(){
	unsigned long currentMillis = millis();
  	if (currentMillis - previousMillis >= intervalSlow) {
      previousMillis = currentMillis;
    if (ledState == LOW) { ledState = HIGH; }
      else { ledState = LOW; }
    digitalWrite(ledPin, ledState); }
}

void fast() {
	unsigned long currentMillis = millis();
  	if (currentMillis - previousMillis >= intervalFast) {
    previousMillis = currentMillis;
    if (ledState == LOW) {  ledState = HIGH; }
      else { ledState = LOW; }
    digitalWrite(ledPin, ledState); }
}

void loop() {  
  Serial.println(bCounter); // for debuging
  buttonState = digitalRead(buttPin); // read the input pin
  if (buttonState == HIGH) { bCounter++; }
  if (bCounter % 2 == 1) { fast(); Serial.println("fast"); //for test
  } else { slow(); Serial.println("slow"); //for test }
  delay(500);
}