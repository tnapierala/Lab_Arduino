void setup() {
  pinMode(8, OUTPUT);  //pin 9 ma pracowac jako wyjscie
  pinMode(9, OUTPUT);  //pin 8 ma pracowac jako wyjscie
  pinMode(6, INPUT_PULLUP); //wew.  ustawienie pull-up.
  pinMode(5, INPUT_PULLUP); //wew.  ustawienie pull-up.
  pinMode(4, INPUT_PULLUP); //wew.  ustawienie pull-up.
  pinMode(3, INPUT_PULLUP); //wew.  ustawienie pull-up.
  //pull-up oznacza odwrócenie logiki przycisków. 
  //WYSOKI, gdy jest otwarty, i NISKI, gdy jest wciśnięty.
}
void loop() {
  int x1 = digitalRead(6); //weryfikacja przycisku 1 (on/off)
  int x2 = digitalRead(5); //weryfikacja przycisku 2 (on/off)
  int x3 = digitalRead(4); //weryfikacja przycisku 3 (on/off)
  int x4 = digitalRead(3); //weryfikacja przycisku 4 (on/off)
  
  // wraunek dla diody zielonej
  if ((!x1||!x2)&&(x2||!x4)&&!x3) {
    digitalWrite(8, LOW);
    // sprawdzenie czy zaświecić, także diodę czerwoną   
    if((!x1||!x2||!x4)&&(x1||!x3||x4)&&(x3||!x4)&&(x2||x3)) {
       digitalWrite(9, LOW);
    }
  } 
  // sprawdzenie czy zaświecić, także diodę czerwoną
  else if ( (!x1||!x2||!x4)&&(x1||!x3||x4)&&(x3||!x4)&&(x2||x3) ) {  
    digitalWrite(9, LOW);
  }
  // jeżeli żaden warunek się nie spełnił to nie załącza żadnej diody 
  else {
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
  }
}