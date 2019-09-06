#define ANALOGIN_PIN 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(112500);
  analogReadResolution(10);
}

void loop() {
  // put your main code here, to run repeatedly:

  int analogin = analogRead(ANALOGIN_PIN);

  Serial.print("Voltagem = ");
  Serial.print(analogin*3.3/1023);
  Serial.println("V");
  
  delay(1000);
}
