int TRIG_PIN = 13;
int ECHO_PIN = 12;

float tempo = 0;
const float velMs = 340;
const float velMm = 0.000340;


void dispara(){
  
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
  }

float Calcula(float temporoso){
  return temporoso * velMm / 2;
}

void pisca(){

  if (digitalRead(LED_BUILTIN) == 1) digitalWrite(LED_BUILTIN, LOW);
  else digitalWrite(LED_BUILTIN, HIGH);
}

float var;

void setup(){
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(100);
}

void loop() {
  dispara();
    tempo = pulseIn(ECHO_PIN, HIGH);
    Serial.println("Distancia : ");
    var = Calcula(tempo);
    Serial.println(var);
    
    if(var < 0.5){ 
      Serial.println("SAI SAI IRMAO TA MT PERTO, PISCAE");
      digitalWrite(LED_BUILTIN, HIGH);
      }else digitalWrite(LED_BUILTIN, LOW);
    delay(2000);

}
