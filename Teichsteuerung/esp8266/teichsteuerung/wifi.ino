#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

//AP definitions
#define ssid "..."
#define password "..."
//IPAddress static_subnet(255,255,255,0);
//IPAddress static_gateway(192,168,0,1);
//IPAddress static_ip(192,168,0,199);
//wird in der folgenden Datei definiert
#include "wifi_secrets.h"

WiFiClient espClient;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(static_ip,static_gateway,static_subnet);
  for (int i=0; i<10; i++)
  {
    if (WiFi.status() == WL_CONNECTED) {      
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    delay(500);
    Serial.print(".");
  }
}

void reconnect() {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Lost WiFi, reconnect...");
    WiFi.disconnect();
    setup_wifi();
  }
}
