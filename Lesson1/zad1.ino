#include <LiquidCrystal.h>
//LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
const int en1 = 12, en2 = 7, button = 6;
bool lcdInit = false;
String date = " 2022/04/12 ";
String time = " 20:21:09 ";
//Sensor
 int TrigHC = 4, EchoHC = 5; long timeHC; int route, wave;

 //Led
 int LedPin = A1, LedValue = A0, readVal = 0;
 byte ledState = LOW;
 long myTime = 0; float voltageLed = 0; 
 unsigned long blinkLED = 700, memoredTimeLED;

 LiquidCrystal lcd1(13, en1, 11, 10, 9, 8);
 LiquidCrystal lcd2(13, en2, 11, 10, 9, 8);

 void setup() {
 pinMode(button,INPUT); pinMode(LedValue, INPUT);
 pinMode(LedPin, OUTPUT); pinMode(TrigHC, OUTPUT);
 pinMode(EchoHC, INPUT);
 }

 void loop(){
 myTime = millis();
 readVal = analogRead(LedValue);
 voltageLed = readVal * (5.0/1023.0);
 
 // czujnik odlegosci
 digitalWrite(TrigHC, LOW);
 delayMicroseconds(2);
 digitalWrite(TrigHC, HIGH);
 delayMicroseconds(10);
 digitalWrite(TrigHC, LOW);
 
 timeHC = pulseIn(EchoHC, HIGH);
 wave = 58; // czas z jakim dzwiek pokonuje odlegosc 1cm
 route = timeHC / wave; // wzor poducenta
 
 if (route < 100) {
 if (myTime - memoredTimeLED >= blinkLED) {
 memoredTimeLED = myTime;
 ledState = !ledState;
 digitalWrite(LedPin, ledState);
 }
 }
 
 if (lcdInit == false) {
 lcdInit = true;
 lcd1.begin(16, 2);
 lcd2.begin(16, 2);
 }
 
 if (digitalRead(button)== HIGH) { 
 lcd2.clear();
 lcd1.print(date);
 lcd1.setCursor(0, 1);
 lcd1.print(time);
 } else {
 lcd1.clear();
 lcd2.print("Distance: ");
 lcd2.print(route);
 lcd2.print(" cm");
 lcd2.setCursor(0, 1);
 lcd2.print("LedVol: ");
 lcd2.print(voltageLed);
 lcd2.print(" V ");
 delay(1000);
 lcd2.clear();
 }
}