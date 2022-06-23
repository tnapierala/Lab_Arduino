int LED = 2;
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED, HIGH);
  delay(500);  Wait for 500 millisecond(s)
  digitalWrite(LED, LOW);
  delay(500);  Wait for 500 millisecond(s)
}
