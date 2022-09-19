/*
 * ESP8266/ESP-01 Proximity based WIFI Door
 * This code is based on kalanda ESP8266 Wifi sniffer https://github.com/kalanda/esp8266-sniffer
 * 
 * The idea is to sniff all packets in a specific channel, since you already know the channel of your AP,
 * so you know where to look. Then check for the RSSI values, a RSSI value of -30 means that the cellphone
 * is closer to the ESP8266 than a value of -50. This is were the TARGET_RSSI come sinto play, we set a value
 * and anything higher it is considered to be in range.
 * Once in range the code will compare the device MAC with string containing all allowed macs, if a match is found
 * then the relay is enabled, opening, for example, a magnetic lock.
 * 
 * Allowed MAC list is downloaded from a remote web server, for this the ESP will first connect to AP, download the MACs then switch to scan mode.
 * 
 * Cellphones does not always send wifi packages, specially if they are already connected to an AP and the cellphone is locked.
 * In android i found that unlocking the phone is enoght for wifi to start transmitting, so unlocking the phone close enoght to the
 * ESP should be enoght to open the door. Failing that opening whatsapp or any app that uses internet should do it.
 * 
 * This code will reset the ESP once an hour to re-download the list of macs. This is the RESET_INTERVAL value, a value of 0 disables the reset.
 * On the ESP-01 TX PIN and built Led pin is the same (GPIO1). Serial output must be disabled if you assign 1 as LED_GPIO. On ESP-01S led is wired to GPIO2.
*/
#define ENABLE_SERIAL
#define TARGET_RSSI -40
#define WIFI_CHANNEL 8
#define RELAY_GPIO 0
#define LED_GPIO 2
#define RELAY_TIME 3000
#define RESET_INTERVAL 60UL*60UL*1000UL
#define STASSID "XXX"
#define STAPSK  "XXX"
/* 
 *  Alternatively, you can comment the call to downloadMacString() function in setup() and just place your MAC addresses 
 *  in the string below, in lowercase, with : and separated by comas. Example: "2c:54:91:88:c9:e3,00:1b:44:11:3a:b7"
*/
String mac_addrs = "";

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "sniffer.h"
bool relay_active = false;
static unsigned long targetMillis = millis() + RESET_INTERVAL;

void downloadMacString()
{
    #ifdef ENABLE_SERIAL
    Serial.print("Connecting to AP...\n");
    #endif
    WiFiClient client;
    HTTPClient http;
    WiFi.begin(STASSID, STAPSK);
    http.setTimeout(5000);
    while ( WiFi.status() != WL_CONNECTED ) { delay(1000); }
    #ifdef ENABLE_SERIAL
    Serial.print("Getting MAC list...\n");
    #endif
    String url = "http://192.168.0.2/macs.php";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "apikey=1234&GetMacs";
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode==200) {
      mac_addrs = http.getString();
      #ifdef ENABLE_SERIAL
      Serial.print("OK\n");
      Serial.print(mac_addrs);
      #endif
    } else {
      #ifdef ENABLE_SERIAL
      Serial.print("Error ");
      Serial.print(httpResponseCode);
      Serial.print("\n Resetting...");
      #endif
      ESP.restart();
    }
    http.end();
    #ifdef ENABLE_SERIAL
    Serial.print("\nDisconnecting...");
    #endif
    WiFi.disconnect();
}

static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length) {
  struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;
  if(snifferPacket->rx_ctrl.rssi >= TARGET_RSSI )
  {
    char addr[] = "00:00:00:00:00:00";
    getMAC(addr, snifferPacket->data, 10);
    #ifdef ENABLE_SERIAL
    Serial.print(addr);
    Serial.print(" NEAR\n");
    #endif
    if(mac_addrs.indexOf(String(addr))>=0)
    {
        if(!relay_active)
        {
          #ifdef ENABLE_SERIAL
          Serial.print("OPEN\n");
          #endif
          relay_active=true;
        }
    }
  }
}

void setup() {
  #ifdef ENABLE_SERIAL
  Serial.begin(115200);
  #endif
  pinMode(RELAY_GPIO,OUTPUT);
  pinMode(LED_GPIO, OUTPUT); 
  delay(10);
  digitalWrite(RELAY_GPIO, HIGH);
  digitalWrite(LED_GPIO, HIGH);
  downloadMacString();
  delay(10);
  #ifdef ENABLE_SERIAL
  Serial.print("\nStart sniffing...\n");
  #endif
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(WIFI_CHANNEL);
  wifi_promiscuous_enable(DISABLE);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(ENABLE);
}

void loop() {
  delay(10);
  if(relay_active)
  {
    digitalWrite(RELAY_GPIO,LOW);
    digitalWrite(LED_GPIO,LOW);
    delay(RELAY_TIME);
    digitalWrite(RELAY_GPIO,HIGH);
    digitalWrite(LED_GPIO, HIGH);
    delay(100);
    relay_active=false; 
  }
  if (RESET_INTERVAL != 0 && millis() > targetMillis) {
    #ifdef ENABLE_SERIAL
    Serial.print("\nResetting to refresh MAC string...\n");
    #endif
    ESP.restart();
  }
}
