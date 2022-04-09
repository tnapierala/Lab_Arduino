const int ledPin =  13;
int ledState = LOW;	// ledState used to set the LED
unsigned long previousMillis = 0;	 // will store last time LED was updated

const long interval = 250;	// interval at which to blink 4Hz

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
}