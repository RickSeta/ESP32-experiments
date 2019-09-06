
#include "WiFi.h"

const char * password = "INSERT_PASSWORD_HERE";

void scanner(int * n){

  Serial.println("Scan start");

    // WiFi.scanNetworks will return the number of networks found
    *n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (*n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(*n);
        Serial.println(" networks found");
        for (int j = 0; j < *n; ++j) {
            // Print SSID and RSSI for each network found
            Serial.print(j + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(j));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(j));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(j) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");
}

int i;
void conexao(){

  int n;
  scanner(&n);
  for (i = 0; i < n; i++){
    
    WiFi.begin(WiFi.SSID(i).c_str(), password);
    Serial.print("Try number: ");
    Serial.println(i);
    delay(5000);
    if(WiFi.status() == WL_CONNECTED){

      Serial.print("This password worked for: ");
      Serial.println(WiFi.SSID(i));
      break;
    }
    
  }

}

void setup() {
  
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");

    conexao();

}

void loop() {
  
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("Connected to: ");
    Serial.println(WiFi.SSID(i));
   
    }
   else Serial.println("Not connected ");
   delay(10000);

}
