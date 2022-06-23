const int LED1 = 12; 
const int LED2 = 11;
 const int LED3 = 10;
unsigned long blinkLED1 = 10000; 
unsigned long blinkLED2 = 8000;
unsigned long currentTime = 0;
unsigned long memoredTimeLED1 = 0; 
unsigned long memoredTimeLED2 = 0; 
unsigned long memoredTimeLED3 = 0;
int stanLED1 = LOW;
int stanLED2 = LOW;
int stanLED3l = LOW;
int stanLED3h = HIGH;
String comdata = "";

void setup(){
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop(){
  currentTime = millis();
  if (currentTime - memoredTimeLED1 >= blinkLED1) {
    memoredTimeLED1 = currentTime;
    stanLED1 = !stanLED1;
    digitalWrite(LED1, stanLED1);
  }
  if (currentTime - memoredTimeLED2 >= blinkLED2) {
    memoredTimeLED2 = currentTime;
    stanLED2 = !stanLED2;
    digitalWrite(LED2, stanLED2);
  }
  if (Serial.available()) {
    comdata = "";
    while (Serial.available()){
		delay(2);
    	comdata += char(Serial.read());
    }
    Serial.println(comdata);
 
  	if ( comdata == "on" || comdata == "On" || comdata == "ON" ) {
   		digitalWrite(LED3, stanLED3h);
   		Serial.println("LED turned ON");
  	}
  	else if ( comdata == "off" || comdata == "Off" || comdata == "OFF" ) {
 		digitalWrite(LED3, stanLED3l);
    	Serial.println("LED turned OFF");
  	}
    else {
    	Serial.println("Wrong command!");
    }
  } 
}
