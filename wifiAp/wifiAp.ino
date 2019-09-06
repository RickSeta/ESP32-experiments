
/******************
 *    INCLUDES    *
 ******************/
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <stdint.h>
#include <stdbool.h>
using namespace std;

/******************
 *    DEFINES     *
 ******************/
#define USE_DISPLAY     //Uncomment to disable display usage
#define USE_BUTTON    // Uncomment to disable button usage
#define BUTTON_PIN    0 // Change this value to match the GPIO value of your board's button pin
#define SDA_PIN       5 // Change these 2 values if your board has different display pins
#define SCL_PIN       4

#ifdef USE_DISPLAY
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA_PIN, SCL_PIN);
#endif

/***************************
 *    GLOBAL VARIABLES     *
 ***************************/
// Raw Beacon Frame
uint8_t beacon_packet[109] = {
  /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: managment beacon frame
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
  /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
  /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source

  // Fixed parameters
  /* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
  /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  /* 32 - 33 */ 0xe8, 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
  /* 34 - 35 */ 0x31, 0x00, // capabilities Tnformation

  // Tagged parameters

  // SSID parameters
  /* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
  /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, // SSID

  // Supported Rates
  /* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
  /* 72 */ 0x82, // 1(B)
  /* 73 */ 0x84, // 2(B)
  /* 74 */ 0x8b, // 5.5(B)
  /* 75 */ 0x96, // 11(B)
  /* 76 */ 0x24, // 18
  /* 77 */ 0x30, // 24
  /* 78 */ 0x48, // 36
  /* 79 */ 0x6c, // 54

  // Current Channel
  /* 80 - 81 */ 0x03, 0x01, // Channel set, length
  /* 82 */      0x01,       // Current Channel

  // RSN information
  /*  83 -  84 */ 0x30, 0x18,
  /*  85 -  86 */ 0x01, 0x00,
  /*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
  /*  91 -  92 */ 0x02, 0x00,
  /*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
  /* 101 - 102 */ 0x01, 0x00,
  /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
  /* 107 - 108 */ 0x00, 0x00
};

const uint8_t attack_time_diff = 100;
const uint8_t wifi_channels[] = {1, 3, 5, 6, 8, 9, 11, 13};
const char beacon_ssids[] = {
  "RPGprr\n"
  "MORTES\n"
  "QueroXP\n"
  "SETHALON\n"
  "VINCENT\n"
  "JAO\n"
  "dMASTER\n"
  "DEDhuman\n"
  "CASAdoPAI\n"
  "VAIcmCALMA\n"
  "TuEva&Adao\n"
  "Tuegay\n"
  "sEnTa\n"
  "MATANCA\n"
  "NOELfdp\n"
  "gaviaoGAY\n"
  "QUARTETO\n"
  "elfosLindos\n"
  "TODOdiaISSO\n"
  "PeidaNao\n"
  "eaeMen\n"
};

char empty_ssid[32];
uint8_t channel_index = 0;
uint8_t mac_addr[6];
uint8_t wifi_channel = 1;
uint8_t packet_size = 0;
uint16_t packet_count = 0;
uint16_t ssids_len = 0;

uint32_t current_time = 0;
uint32_t last_attack = 0;
uint32_t last_draw = 0;
uint32_t last_press = 0;

bool its_spammin_time = false;
bool state_changed = true;
bool button_pressed = false;

/*****************************
 *   FUNCTION DEFINITIONS    *
 *****************************/
esp_err_t event_handler(void *context, system_event_t *event)
{
  return ESP_OK;
}

void generate_mac()
{
  uint8_t i;
//  randomSeed(random());
  delay(10);
  for (i = 0; i < 6; i++)
    mac_addr[i] = random(256);
}

void setup()
{
  // Temp variables
  uint8_t i;
  
  // Init serial
  Serial.begin(115200);
  Serial.print("BS32: Begin setup");
  
  // ESP32 system init
  nvs_flash_init(); // onboard flash storage
  tcpip_adapter_init(); // tcp ip stack (needed for DHCP server and handling wifi events)
  
  // Generate random mac address
  generate_mac();
  
  // Set packet size (currently no WPA)
  packet_size = sizeof(beacon_packet);
  beacon_packet[34] = 0x21;
  packet_size -= 26;
  
  // Fill empty SSID
  for (i = 0; i < sizeof(empty_ssid, sizeof(char)); i++) empty_ssid[i] = ' '; // Fill empty SSID
  Serial.print(".");
  
  // Wifi init
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  ESP_ERROR_CHECK(esp_wifi_init(&config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  Serial.print(".");
  
  
#ifdef USE_BUTTON
  // Init IO
  pinMode(BUTTON_PIN, INPUT_PULLUP);
#else
  its_spammin_time = true; // It's always spammin time >:]
#endif
  
#ifdef USE_DISPLAY
  // Init display
  display.init();
  
  // Draw initial splash screen
  display.clear();  
#endif

  ssids_len = strlen_P(beacon_ssids);
  
  Serial.println(" Done!!");
}

void loop()
{
  current_time = millis();
  
  if (its_spammin_time)
  {
    // Time to spam some beacon frames!
    if (current_time - last_attack > attack_time_diff)
    {
      Serial.println("BS32: Broadcasting...");
      
      // Temp variaibles
      uint8_t i = 0, j = 0, k = 0, ssid_number = 1, ssid_len;
      char tmp;

      // Set wifi channel
      wifi_channel = wifi_channels[channel_index];
      esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);

      // Set beacon packet's wifi channel
      beacon_packet[82] = wifi_channel;

      delay(2); // Adding this seems to reduce 'wifi out of memory' errors (gives ESP32 more time to reallocate memory for variables in each loop iteration?)
      while (i < ssids_len)
      {
        // Read out next ssid
        do {
          tmp = pgm_read_byte(beacon_ssids + i + j);
          j++;
        } while (tmp != '\n' && j <= 32);
        
        ssid_len = j - 1;

        // Set MAC address
        mac_addr[5] = ssid_number;
        ssid_number++;

        // Write MAC address into beacon frame
        memcpy(&beacon_packet[10], mac_addr, 6);
        memcpy(&beacon_packet[16], mac_addr, 6);

        // Reset SSID in beacon frame
        memcpy(&beacon_packet[38], empty_ssid, 32);

        // Write new SSID into beacon frame
        memcpy_P(&beacon_packet[38], &beacon_ssids[i], ssid_len);

        for (k = 0; k < 3; k++)
        {
          packet_count += esp_wifi_80211_tx(WIFI_IF_AP, beacon_packet, packet_size, false) == ESP_OK;
          delay(1);
        }
        
        i += j;
        j = 0;
      }
     
      channel_index++;
    }

    // Draw packet rate each second
    if (current_time - last_draw > 1000)
    {
      Serial.print("BS32: Packet (/s) = ");
      Serial.println(packet_count);
      
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(6, 6, "BeaconSpam32");
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(6, 34, "State = [enabled]");
      display.drawString(6, 44, "Packets /s: " + (String) packet_count);
      display.display();
      
      last_draw = current_time;
      packet_count = 0;
    }
  }
  
  else
  {
    // Only outputs 'disabled' if state has recently changed, otherwise no need!
    if (state_changed) {
      Serial.println("BS32: Currently disabled");
#ifdef USE_DISPLAY
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(6, 6, "BeaconSpam32");
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(6, 34, "State = [disabled]");
      display.display();
#endif
    
    state_changed = false;
    }
  }

#ifdef USE_BUTTON
  // Check button input
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    // Handles case where button already pressed (so held down??)
    if (!button_pressed)
    {
      if (its_spammin_time) its_spammin_time = false;
      else {
        its_spammin_time = true;
        state_changed = true;
      }

      last_press = current_time;
      button_pressed = true;
    }
  }
  else button_pressed = false;
#endif
}
