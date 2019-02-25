#define WIFI_ENABLED

#include <math.h>
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w

// Fonts
#include "Ubuntu_C32pt8b.h"
#include "Ubuntu_C16pt8b.h"

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include<ArduinoJson.h>

#include "DHT.h"

#define DHTPIN D6
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#if defined(ESP8266)

// generic/common.h
//static const uint8_t SS    = 15; // D8 
//static const uint8_t MOSI  = 13; // D7 
//static const uint8_t MISO  = 12; // D6
//static const uint8_t SCK   = 14; // D5

GxIO_Class io(SPI, /*CS=D8 orange*/ SS, /*DC=D3 green*/ 0, /*RST=D4 white*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io /*RST=D4 white*/ /*BUSY=D2 violet*/); // default selection of D4(=2), D2(=4)

#elif defined(ESP32)

// pins_arduino.h, e.g. LOLIN32
//static const uint8_t SS    = 5;
//static const uint8_t MOSI  = 23;
//static const uint8_t MISO  = 19;
//static const uint8_t SCK   = 18;

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

#endif

#if defined (ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#include <WiFiClient.h>

#include "secrets.h"

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PWD;
const char *owmKey = OWM_APPID;
const char *owmLocation = OWM_LOCATION_REQUEST_PART;

WiFiClient client;
HTTPClient http;

int connected = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  dht.begin();
  display.init(115200); // enable diagnostic output on Serial

#ifdef WIFI_ENABLED
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  int ConnectTimeout = 30; // 15 seconds
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
    if (--ConnectTimeout <= 0)
    {
      Serial.println();
      Serial.println("WiFi connect timeout");
      return;
    }
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  connected = 1;

  // Print the IP address
  Serial.println(WiFi.localIP());
#endif

  Serial.println("setup done");
}

float lastTemp = -100;
float lastHumidity = -1;
float lastOutTemp = -100;
float lastOutPressure = -1;
float outTemp = -101;
float outPressure = -1;

const int loopWaitTimeMS = 10000;
const int loopHttpAccessTimeMS = 300000;
int nextHttpAccessCounter = 0;

void loop() {
#ifdef WIFI_ENABLED
  if (connected) {
    if (nextHttpAccessCounter <= 0) {
      if (http.begin(String("http://api.openweathermap.org/data/2.5/weather?") + owmLocation + "&units=metric&lang=de&appid=" + owmKey)) {
        Serial.println("Requesting OWM");
        int httpCode = http.GET();
        if (httpCode > 0) {
          Serial.print("Response code: "); Serial.println(httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String data = http.getString();
            Serial.println("Parsing OWM Data");
            DynamicJsonBuffer json;
            JsonObject &root = json.parseObject(data);
            if (root.success()) {
              outTemp = root["main"]["temp"];
              outPressure = root["main"]["pressure"];
              Serial.print("OWM temperature: ");
              Serial.println(outTemp);
              Serial.print("OWM humidity: ");
              Serial.println(outPressure);
              outPressure = round(outPressure);
  
              nextHttpAccessCounter = loopHttpAccessTimeMS / loopWaitTimeMS;
            } else Serial.println("Parsing failed");
            Serial.println("OWM data parsed");
          }
        } else Serial.println("Response code is <= 0");
        http.end();
      } else Serial.println("http.begin() failed");
    } else nextHttpAccessCounter--;
  }
#endif

  float temp = readTemperature();
  Serial.print("Local temperature: "); Serial.println(temp);
  temp = round(temp * 2.0f) / 2.0f; 
  float hum = readHumidity();
  Serial.print("Local humidity: "); Serial.println(hum);
  hum = round(hum);
  if (temp != lastTemp || hum != lastHumidity || outTemp != lastOutTemp || outPressure != lastOutPressure) {
    display.setRotation(3);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    
    if (!isnan(temp)) {
    display.setFont(&Ubuntu_C32pt8b);
      display.setCursor(178 - getWidth(temp), 55);
      display.print((double)temp, 1);
      display.print("\260C ");
    }
    if (!isnan(hum)) {
      display.setCursor(190, 45);
      display.setFont(&Ubuntu_C16pt8b);
      display.print((double)hum, 0);
      display.print("%");
    }

#ifdef WIFI_ENABLED
    if (!isnan(outTemp)) {
      display.setFont(&Ubuntu_C32pt8b);
      display.setCursor(178 - getWidth(outTemp), 115);
      display.print((double)outTemp, 1);
      display.print("\260C ");
    }
    if (!isnan(outPressure)) {
      display.setCursor(190, 105);
      display.setFont(&Ubuntu_C16pt8b);
      display.print((double)outPressure, 0);
      display.print("hPa");
    }
#endif    

    display.update();
    
    lastTemp = temp;
    lastHumidity = hum;
    lastOutTemp = outTemp;
    lastOutPressure = outPressure;
  }

  delay(loopWaitTimeMS);
}

float readTemperature()
{
  float temp = dht.readTemperature();
  return temp;
}

float readHumidity()
{
  float hum = dht.readHumidity();
  return hum;
}

int getWidth(float temp)
{
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
    String t = "9.9\260C";
    if (abs(temp) >= 10.0f) t = "9" + t;
    if (temp < 0.0f) t = "-" + t;
    display.getTextBounds(t.c_str(), 0, 55, &x, &y, &w, &h);
//    Serial.printf("x=%d, y=%d, w=%d, h=%d\n", x, y, w, h);
    return x + w;
}
