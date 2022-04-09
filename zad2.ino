int ledRed = 10;
int ledYellow = 11;
int ledGreen = 12;

void setup() {                
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT); 
}
 
void loop() {
  digitalWrite(ledRed, HIGH);       //włączenie diody czerwonej
  delay(4000);                   //zatrzymanie stanu na 3 sekundy
  digitalWrite(ledYellow, HIGH);    //włączenie diody żółtej
  delay(1500);                   //zatrzymanie stanu na 1 sekundę 
  digitalWrite(ledRed, LOW);        //wyłączenie diody czerwonej
  digitalWrite(ledYellow, LOW);     //wyłączenie diody żółtej
  digitalWrite(ledGreen, HIGH);     //włączenie diody zielonej
  delay(4000);                   //zatrzymanie stanu na 2 sekundy
  digitalWrite(ledGreen, LOW);      //wyłączenie diody zielonej
  digitalWrite(ledYellow,HIGH);     //włączenie diody żółtej
  delay(2200);                   //zatrzymanie stanu na 1 sekundę
  digitalWrite(ledYellow,LOW);      //wyłączenie diody żółtej
}