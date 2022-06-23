#include  <ESP8266WiFi.h>
#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

const char* ssid     ="ASUS"; // Tu wpisz nazwę swojego wifi
const char* password = "CDEFC56CA"; // Tu wpisz hasło do swojego wifi

// Web Server on port 80
WiFiServer server(80); String head;
// Auxiliar variables to store the current output state
String output15State = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

#define LED D4 //LED NA PLYTCE
// zmiany coloru led
#define PIN D5   // input pin Neopixel is attached to
#define NUMPIXELS 18 // number of neopixels in strip

//Zdefiniowane zmienne do segmentów led
#define PIXELS_PER_SEGMENT 1  // Number of LEDs in each Segment
#define PIXELS_DIGITS 4       // Number of connected Digits
#define PIXELS_PIN2 D8        // GPIO Pin
#define PIXEL_DASH D7        // GPIO Pin

int delayval = 25; // timing delay in milliseconds
int redColor = 0; int greenColor = 0; int blueColor = 0; int color = 0;
uint32_t hue;

//zmienne do czujnaika temperatury
#define DHTTYPE DHT22 //moj czujnik
const int DHTPin = 5; // DHT Sensor
DHT dht(DHTPin, DHTTYPE); 
float TEMP; int realTemp; // Initialize DHT sensor.
float hic; int touchTemp; float h;
// Temporary variables
static char celsiusTemp[7];

//fotorezystor
#define fRezy D2
int pomiarSwiatla; int swiatlo;

//Time
#define TIME_FORMAT        12    // 12 = 12 hours format || 24 = 24 hours format 
WiFiUDP ntpUDP;
// 'time.nist.gov' is used (default server) with +1 hour offset (3600 seconds) 60 seconds (60000 milliseconds) update interval
NTPClient timeClient(ntpUDP, "time.nist.gov", 7200, 60000); //GMT+5:30 : 5*3600+30*60=19800

int period = 1000;   //Update frequency
unsigned long time_now = 0;
int Second, Minute, Hour;

//LEDY
int ledPin = 12; //D6 ustawiamy, do którego pinu jest podłączona dioda/
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS_PER_SEGMENT * 7 * PIXELS_DIGITS, PIXELS_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(PIXELS_PER_SEGMENT * 7 * PIXELS_DIGITS, PIXELS_PIN2, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(PIXELS_PER_SEGMENT, PIXEL_DASH, NEO_RGB + NEO_KHZ800);

//Pixel Arrangement
/*
          b
        a   c
          g
        f   d
          e
*/

// Segment array
byte segments[7] = {
  //abcdefg
  0b0000001,     // Segment g
  0b0000010,     // Segment f
  0b0000100,     // Segment e
  0b0001000,     // Segment d
  0b0010000,     // Segment c
  0b0100000,     // Segment b
  0b1000000      // Segment a
};

//Digits array
byte digits[11] = {
  //abcdefg
  0b1111110,     // 0
  0b0011000,     // 1
  0b0110111,     // 2
  0b0111101,     // 3
  0b1011001,     // 4
  0b1101101,     // 5
  0b1101111,     // 6
  0b0111000,     // 7
  0b1111111,     // 8
  0b1111001,     // 9
  0b1100110     // C
};

void setup(){
  Serial.begin(115200);
  
  Serial.println();
  
  pixels.begin();
  strip2.begin();
  strip3.begin();

  dht.begin();

  pinMode(LED, OUTPUT); // ustawiamy pin jako wyjście

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(2000);
  Serial.println(WiFi.localIP()); // Printing the ESP IP address

  timeClient.begin(); // time begin
  delay(100);
}
     
void loop(){
  digitalWrite(LED, LOW);    // wyłączamy diodę, podajemy stan niski

//Time
  

//Temp & color led 
  odczytTMP();
  checkColor();
  fRez();

//HTML
  servHtml();
//Show TEMP when click on the page site button on "ON"  
  if (output15State=="on") {
    clearDisplay();
    delay(10);
    displayColor();
    myDigit(100);
    delay(5000);
    clearDisplay();
    delay(10);
    timeLoop();
    delay(5000);
  }

// Digit
  //disp_Seg(200);                // Cycle through all segments        (DelayTime)
  //disp_Digits(500);            // Show digits from 0-9              (DelayTime)
  //clearDisplay();

}  

void timeLoop() {
  if (WiFi.status() == WL_CONNECTED) { // check WiFi connection status
      timeClient.update();
      int Hours;
      unsigned long unix_epoch = timeClient.getEpochTime();   // get UNIX Epoch time
      Second = second(unix_epoch);                            // get seconds
      Minute = minute(unix_epoch);                            // get minutes
      Hours  = hour(unix_epoch);                              // get hours
  
      if (TIME_FORMAT == 12) {
        if (Hours > 12) {
          Hour = Hours - 12;
        }
        else
          Hour = Hours;
      }
      else
        Hour = Hours;
    }

    while (millis() > time_now + period) {
      time_now = millis();
      disp_Time();     // Show Time
    Serial.println();
    Serial.print("Time: ");
    Serial.print(Hour);
    Serial.print(" : ");
    Serial.print(Minute);
    Serial.print(" : ");
    Serial.print(Second);
    }
}

void servHtml() {
    WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {
    Serial.println();
    Serial.println("New client");
    // bolean to locate when the http request ends
    //boolean blank_line = true;
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        head += c;
        if (c == '\n') {
          if (currentLine.length() == 0){
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = TEMP;
            // Check if any reads failed and exit early (to try again).
            if (isnan(t)) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");  
            } else {
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              dtostrf(hic, 6, 2, celsiusTemp);             
              // You can delete the following Serial.print's, it's just for debugging purposes
              Serial.println();
              Serial.print("Heat index: ");
              Serial.print(hic);
              Serial.println(" *C ");
              Serial.print("Temperature: ");
              Serial.print(t);
              Serial.println(" *C ");
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.println(" % ");
            }
            
             // turns the GPIOs on and off
            if (head.indexOf("GET /15/on") >= 0) {
              Serial.println();
              Serial.println("GPIO 15 on");
              output15State = "on";
            } else if (head.indexOf("GET /15/off") >= 0) {
              Serial.println();
              Serial.println("GPIO 15 off");
              output15State = "off";
              onOffLED();
            }
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature and humidity
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta http-equiv='refresh' content='60'>");
            client.println("<script src='https://kit.fontawesome.com/a1ae2e1773.js' crossorigin='anonymous'></script>");
            client.println("<style> body {font-family: 'Lato', sans-serif;text-align: center;background-color: #888;}");
            client.println("h3 {color:#333;padding: 2%;font-weight: normal;} i {font-size: 80px;padding-left: 2%;}");
            client.println(".button {padding: 15px 25px;font-size: 24px;cursor: pointer;color: #fff; border: none;border-radius: 15px;box-shadow: 0 9px #999;}");
            client.println(".button:hover {background-color: #555} .button:active {box-shadow: 0 5px #666;transform: translateY(4px);}");
            client.println(".on {background-color: #4CAF50;}.off {background-color: red;}");
            client.println(".flex{display: flex;font-size: 20px; width: 100%; margin:auto; align-items: center; justify-content: center;align-content: center;flex-wrap:wrap;}");
            client.println(".flex.b{ margin-top: 2%; } .btn{ margin: 3%; font-weight: bold; font-size: 30px; } .title{ flex-direction: column; margin-top: 1%;}");
            client.println(".title h1 { font-size: 30px; margin: 1%; } .tempHI i { color: yellowgreen; } .temp i {color: orange;} .hum i {color: #00add6;}");
            client.println(".sun i {color: yellow;} b {color: #fff;} .b .ledon {color: #ffef00;} .b .ledoff {color: black;} </style> </head> <body>");
            client.println("<div class='flex title'><h1>ESP8266</h1><h1>Temperature and Humidity</h1></div>");
            client.println("<div class='flex b'> ");
            // If the output5State is off, it displays the ON button       
            if (output15State=="off") {// If the output5State is off, it displays the ON button       
              client.println("<i class='fa-regular fa-lightbulb ledoff'></i> <div class='text btn'> GPIO D8 - LED </div> ");
              client.println("<a href='/15/on'> <button class='button btn on'> ON </button> </a>");
            } else {
              client.println("<i class='fa-solid fa-lightbulb ledon'></i> <div class='text btn'> GPIO D8 - LED </div> ");
              client.println("<a href='/15/off'> <button class='button btn off'> OFF </button> </a>");
            }
            client.println("</div>");
            client.println("<div class='flex tempHI'> <i class='fa-solid fa-temperature-high'></i> <h3> Temperature in HeatIndex: <b>");
            client.println(celsiusTemp);
            client.println(" *C </b> </h3> </div>");
            client.println("<div class='flex temp'><i class='fa-solid fa-temperature-half'></i><h3> Temperature in Celsius: <b> ");
            client.println(TEMP);
            client.println(" *C </b></h3> </div>");
            client.println("<div class='flex hum'> <i class='fa-solid fa-droplet'></i> <h3>Temperature in Humidity: <b> ");
            client.println(h);
            client.println(" % </b></h3></div>");
            client.println("<div class='flex sun'><i class='fa-solid fa-sun'></i><h3>Fotorezistor diode status: <b> ");
            client.println(swiatlo);
            client.println("</b></h3></div>");
            client.println("<div class='flex sun'><i class='fa-solid fa-clock'></i><h3>Time status: <b> ");
            client.println(Hour); client.print(" : "); client.print(Minute); client.print(" : "); client.print(Second);
            client.println("</b></h3></div>");
            client.println("</body></html>");     
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          // when finds a character on the current line
          //blank_line = false;
        }
      }
    }
    // Clear the header variable
    head = "";  
    // closing the client connection
    client.stop();
    Serial.println();
    Serial.println("Client disconnected.");
  }
}
  
void odczytTMP() { 
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  TEMP = dht.readTemperature();
  realTemp = round(TEMP);
  hic = dht.computeHeatIndex(TEMP, h, false);
  touchTemp = round(hic);
  
  Serial.println();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if ( isnan(TEMP)&& isnan(hic) ) {
    Serial.println("Błąd czujnika DHT!"); 
  } else {
    Serial.print("HeatIndex: ");
    Serial.print(hic);
    Serial.println(" *C");
    Serial.print("Temperatura: ");
    Serial.print(TEMP);
    Serial.println(" *C");
  }
  delay(1000); //opóźnienie między kolejnymi odczytami - 1 s
}

void fRez () {
  pomiarSwiatla = analogRead(fRezy); // wskazanie pinu czytającego dane analogowe
  swiatlo = map(pomiarSwiatla, 0, 1023, 0, 1);
  Serial.println("");
  Serial.print("Swiatlo: ");
  Serial.println(swiatlo);
  delay(500);
}

void checkColor() {
  if (touchTemp < 19 ) {
    Serial.println();
    Serial.print(" < 19 ");
    Serial.println();
    color = 1;
  }
  else if( touchTemp > 20 && touchTemp < 27 ) {
    Serial.println();
    Serial.print(" 20 - 26");
    Serial.println();
    color = 2; 
  }
  else {
    Serial.println();
    Serial.print(" > 26 ");
    Serial.println();
    color = 3;
  }
}

void displayColor() {
  setColor();
  delay(500);
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
    // This sends the updated pixel color to the hardware.
    pixels.show();
    // Delay for a period of time (in milliseconds).
    delay(delayval);
  }
  delay(1500);
}

void setColor(){
  if (color == 1) {
    if (swiatlo == 0) {
      redColor = 0;
      greenColor = 64;
      blueColor = 255;
    } else {
      redColor = 0;
      greenColor = 4;
      blueColor = 16;
    }
  } else if (color == 2) {
    if (swiatlo == 0) {
      redColor = 0;
      greenColor = 255;
      blueColor = 0;
    } else {
      redColor = 0;
      greenColor = 16;
      blueColor = 0;
    }
  } else if (color == 3) {
    if (swiatlo == 0) {
      redColor = 255;
      greenColor = 0;
      blueColor = 0;
    } else {
      redColor = 16;
      greenColor = 0;
      blueColor = 0;
    }
  }
  delay(500);
}

void getColor(){
  if (color == 1) { 
    if (swiatlo == 0) {
      hue = strip2.Color(0, 64,  255);
    } else {
      hue = strip2.Color(0, 4, 16);
    }
  }
  else if (color == 2) {    
    if (swiatlo == 0) {
      hue = strip2.Color(0, 255,  0);
    } else {
      hue = strip2.Color(0, 16, 0);
    }
  }
  else if (color == 3) {     
    if (swiatlo == 0) {
      hue = strip2.Color(255, 0,  0);
    } else {
      hue = strip2.Color(16, 0, 0);
    }
  }
  delay(500);
}

// Digit Leds - TEMP
void clearDisplay() {
  for (int i = 0; i < strip2.numPixels(); i++) {
    strip2.setPixelColor(i, strip2.Color(0, 0, 0));
  }
  strip2.show();
}

void onOffLED() {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
    delay(delayval);
  }
  for (int i = 0; i < strip2.numPixels(); i++) {
    strip2.setPixelColor(i, strip2.Color(0, 0, 0));
    strip2.show();
    delay(delayval);
  }
  for (int i = 0; i < strip2.numPixels(); i++) {
    strip3.setPixelColor(i, strip3.Color(0, 0, 0));
    strip3.show();
    delay(delayval);
  }
}

void disp_Seg(int wait) {
  for (int d = 0; d < 2; d++) {
    for (int i = 7; i > 0; i--) {
      for (int n = 0; n < PIXELS_DIGITS; n++) {
        writeSegment(n, i);
      }
      strip2.show();
      delay(wait);
    }
  }
  clearDisplay();
}

void disp_Digits(int wait) {
  for (int i = 0; i < 10; i++) {
    for (int n = 0; n < PIXELS_DIGITS; n++) {
      writeDigit(n, i);
    }
    strip2.show();
    delay(wait);
  }
  clearDisplay();
}

void writeDigit(int index, int value) {
  //Serial.print("digit_hue_"); Serial.print(hue); Serial.print("_");
  //Serial.print(value); Serial.print("_");
  getColor();
  byte digit = digits[value];
  for (int i = 6; i >= 0; i--) {
    int offSet = index * (PIXELS_PER_SEGMENT * 7) + i * PIXELS_PER_SEGMENT;
    uint32_t color1;
    if (digit & 0x01 != 0) {
      // rainbow color digits
//       if (value == 1) color1 = strip2.Color(50, 0,  0);
//       if (value == 2) color1 = strip2.Color(50, 50, 0);
//       if (value == 3) color1 = strip2.Color(50, 0, 50);
//       if (value == 4) color1 = strip2.Color(0, 50,  0); 
//       if (value == 5) color1 = strip2.Color(0, 50, 50);
//       if (value == 6) color1 = strip2.Color(0,  0, 50);
//       if (value == 7) color1 = strip2.Color(50, 25, 0);
//       if (value == 8) color1 = strip2.Color(25, 5, 75);
//       if (value == 9) color1 = strip2.Color(75, 25, 5);
//       if (value == 0) color1 = strip2.Color(5, 75, 25);

      // one color digits by temp
      if (value == 1) color1 = hue;
      if (value == 2) color1 = hue;
      if (value == 3) color1 = hue;
      if (value == 4) color1 = hue;
      if (value == 5) color1 = hue;
      if (value == 6) color1 = hue;
      if (value == 7) color1 = hue;
      if (value == 8) color1 = hue;
      if (value == 9) color1 = hue;
      if (value == 0) color1 = hue;
      if (value == 10) color1 = hue;
    }
    else {
      color1 = strip2.Color(0, 0, 0);
    }
    
    for (int j = offSet; j < offSet + PIXELS_PER_SEGMENT; j++) {
      strip2.setPixelColor(j, color1);
    }
    
    digit = digit >> 1;
  }
}

void writeSegment(int index, int value) {
  byte seg = segments[value];
  for (int i = 6; i >= 0; i--) {
    int offSet = index * (PIXELS_PER_SEGMENT * 7) + i * PIXELS_PER_SEGMENT;
    uint32_t color1;
    if (seg & 0x01 != 0) {
      if (value == 0) color1 = strip2.Color(50, 0, 0);
      if (value == 1) color1 = strip2.Color(0, 50, 50);
      if (value == 2) color1 = strip2.Color(0, 50, 0);
      if (value == 3) color1 = strip2.Color(50, 0, 50);
      if (value == 4) color1 = strip2.Color(50, 50, 50);
      if (value == 5) color1 = strip2.Color(0, 0, 50);
      if (value == 6) color1 = strip2.Color(50, 50, 0);
      if (value == 6) color1 = strip2.Color(0, 0, 50);
    }
    else
      color1 = strip2.Color(0, 0, 0);

    for (int j = offSet; j < offSet + PIXELS_PER_SEGMENT; j++) {
      strip2.setPixelColor(j, color1);
    }
    seg = seg >> 1;
  }
}

void writeMinus(int wait) {
  uint32_t color1;
  if (swiatlo == 0) {
      color1 = strip3.Color(0, 64,  255);
    } else {
      color1 = strip3.Color(0, 4,  16);
    }
  
  for (int i=0; i < 5; i++) {
    strip3.setPixelColor( i, color1 );
  }
  strip3.show();
  delay(wait);
}

// DIGIT AND TEMP
void myDigit(int wait) {
  Serial.println();
  Serial.println("LEDY_");
  switch (touchTemp) {
    case -25:  writeDigit(0,2);  writeDigit(1,5); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -24:  writeDigit(0,2);  writeDigit(1,4); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -23:  writeDigit(0,2);  writeDigit(1,3); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -22:  writeDigit(0,2);  writeDigit(1,2); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -21:  writeDigit(0,2);  writeDigit(1,1); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -20:  writeDigit(0,2);  writeDigit(1,0); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -19:  writeDigit(0,1);  writeDigit(1,9); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -18:  writeDigit(0,1);  writeDigit(1,8); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -17:  writeDigit(0,1);  writeDigit(1,7); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -16:  writeDigit(0,1);  writeDigit(1,6); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -15:  writeDigit(0,1);  writeDigit(1,5); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -14:  writeDigit(0,1);  writeDigit(1,4); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -13:  writeDigit(0,1);  writeDigit(1,3); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -12:  writeDigit(0,1);  writeDigit(1,2); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -11:  writeDigit(0,1);  writeDigit(1,1); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -10:  writeDigit(0,1);  writeDigit(1,0); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -9:   writeDigit(0,-1);  writeDigit(1,9); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -8:   writeDigit(0,-1);  writeDigit(1,8); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -7:   writeDigit(0,-1);  writeDigit(1,7); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -6:   writeDigit(0,-1);  writeDigit(1,6); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -5:   writeDigit(0,-1);  writeDigit(1,5); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -4:   writeDigit(0,-1);  writeDigit(1,4); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -3:   writeDigit(0,-1);  writeDigit(1,3); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -2:   writeDigit(0,-1);  writeDigit(1,2); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case -1:   writeDigit(0,-1);  writeDigit(1,1); writeDigit(2, 10); writeMinus(100); strip2.show(); delay(wait);  break;
    case 0:    writeDigit(0,-1);  writeDigit(1,0); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 1:    writeDigit(0,-1);  writeDigit(1,1); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 2:    writeDigit(0,-1);  writeDigit(1,2); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 3:    writeDigit(0,-1);  writeDigit(1,3); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 4:    writeDigit(0,-1);  writeDigit(1,4); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 5:    writeDigit(0,-1);  writeDigit(1,5); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 6:    writeDigit(0,-1);  writeDigit(1,6); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 7:    writeDigit(0,-1);  writeDigit(1,7); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 8:    writeDigit(0,-1);  writeDigit(1,8); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 9:    writeDigit(0,-1);  writeDigit(1,9); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 10:   writeDigit(0,1);  writeDigit(1,0); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 11:   writeDigit(0,1);  writeDigit(1,1); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 12:   writeDigit(0,1);  writeDigit(1,2); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 13:   writeDigit(0,1);  writeDigit(1,3); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 14:   writeDigit(0,1);  writeDigit(1,4); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 15:   writeDigit(0,1);  writeDigit(1,5); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 16:   writeDigit(0,1);  writeDigit(1,6); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 17:   writeDigit(0,1);  writeDigit(1,7); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 18:   writeDigit(0,1);  writeDigit(1,8); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 19:   writeDigit(0,1);  writeDigit(1,9); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 20:   writeDigit(0,2);  writeDigit(1,0); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 21:   writeDigit(0,2);  writeDigit(1,1); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 22:   writeDigit(0,2);  writeDigit(1,2); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 23:   writeDigit(0,2);  writeDigit(1,3); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 24:   writeDigit(0,2);  writeDigit(1,4); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 25:   writeDigit(0,2);  writeDigit(1,5); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 26:   writeDigit(0,2);  writeDigit(1,6); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 27:   writeDigit(0,2);  writeDigit(1,7); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 28:   writeDigit(0,2);  writeDigit(1,8); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 29:   writeDigit(0,2);  writeDigit(1,9); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 30:   writeDigit(0,3);  writeDigit(1,0); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 31:   writeDigit(0,3);  writeDigit(1,1); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 32:   writeDigit(0,3);  writeDigit(1,2); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 33:   writeDigit(0,3);  writeDigit(1,3); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 34:   writeDigit(0,3);  writeDigit(1,4); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 35:   writeDigit(0,3);  writeDigit(1,5); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 36:   writeDigit(0,3);  writeDigit(1,6); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 37:   writeDigit(0,3);  writeDigit(1,7); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 38:   writeDigit(0,3);  writeDigit(1,8); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 39:   writeDigit(0,3);  writeDigit(1,9); writeDigit(2, 10); strip2.show(); delay(wait);  break;
    case 40:   writeDigit(0,4);  writeDigit(1,4); writeDigit(2, 10); strip2.show(); delay(wait);  break;
  }

  // for (int i = 0; i < 10; i++) {
  //   for (int n = 0; n < PIXELS_DIGITS; n++) {
  //     writeDigit(n, i);
  //       Serial.println("");
  //       Serial.println("dN: ");
  //      Serial.println(n);
  //       Serial.println("dI: ");
  //      Serial.println(i);
  //       Serial.println("");
  //   }
  //   strip2.show();
  //   delay(wait);
  // }

}

// TIME WRITE

void disp_Time() {
  writeTime(0, Hour / 10);
  writeTime(1, Hour % 10);
  writeTime(2, Minute / 10);
  writeTime(3, Minute % 10);
  writeDots(100);
  strip2.show();
}

void writeDots(int wait) {
  uint32_t color1;
  if (swiatlo == 0) {
      color1 = strip3.Color(0, 64,  255);
    } else {
      color1 = strip3.Color(0, 4,  16);
    }
  
  for (int i=0; i < 5; i++) {
    strip3.setPixelColor( i, color1 );
  }
  strip3.show();
  delay(wait);
}

void writeTime(int index, int val) {
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1 ) margin = 0;
  if (index == 2 || index == 3 ) margin = 1;
  if (index == 4 || index == 5 ) margin = 2;
  for (int i = 6; i >= 0; i--) {
    int offset = index * (PIXELS_PER_SEGMENT * 7) + i * PIXELS_PER_SEGMENT;
    uint32_t color;
    if (digit & 0x01 != 0) {
      if (index == 0 || index == 1 ) color = strip2.Color(16, 0,  0);
      if (index == 2 || index == 3 ) color = strip2.Color(0, 16,  0);
      if (index == 4 || index == 5 ) color = strip2.Color(0,  0, 16);
    }
    else
      color = strip2.Color(0, 0, 0);

    for (int j = offset; j < offset + PIXELS_PER_SEGMENT; j++) {
      strip2.setPixelColor(j, color);
    }
    digit = digit >> 1;
  }
}
