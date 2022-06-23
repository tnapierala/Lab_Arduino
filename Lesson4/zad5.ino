int val = 0 ;
void setup() {
  pinMode(A0,INPUT);
  pinMode(9,OUTPUT);
  Serial.begin(9600);
}
void loop() {
  val = analogRead(A0); // odczyt wartości potencjometru
  analogWrite(9, val/4); //odczyt zakresu do 255
  Serial.println(val);
}
