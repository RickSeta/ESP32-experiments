
#include "BluetoothSerial.h"
#include <string.h>

//objSerial.read();  objSerial.write(); objSerial.println(); objSerial.readString(); objSerial.available();
BluetoothSerial objSerial;

void acende(){
  
    digitalWrite(LED_BUILTIN,  HIGH);
  }

void apaga(){

    digitalWrite(LED_BUILTIN, LOW);
  }

void pisca(){
  
    delay(1000);
    acende();
    delay(1000);
    apaga();
    
  }
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  objSerial.begin("rickseta");
}

void checaModo(String modo){
  
    if (modo.equals("acende\n")){

        acende();
      }
    else if(modo.equals("apaga\n")){

        apaga();
      }
    else if(modo.equals("pisca\n")) pisca();
  }

 int entrada;

void loop() {
  if(Serial.available())
      objSerial.println(Serial.readString());

  if(objSerial.available()){
      
      entrada = objSerial.read();
      Serial.println(entrada);
  }

  checaModo(entrada);
  
  delay(1000);

}
