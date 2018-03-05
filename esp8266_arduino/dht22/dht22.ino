#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// WLAN ssid
#define WLAN_SSID ""
// WLAN passphrase
#define WLAN_PASS ""
// Thingsboard server name/IP address
#define THINGSBOARD_SERVER ""
// Thingsboard port
#define THINGSBOARD_PORT 1883
// Thingsboard device token
#define TOKEN ""

DHT dht(2, DHT22);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastMeasured;

IPAddress staticIP(192, 168, 211, 102);
IPAddress gateway(192, 168, 211, 1);
IPAddress subnet(255, 255, 255, 0);

void readTemperatureAndHumidity() {
  Serial.println("Get temperature and humidity");

  float h = 0;
  for (int i = 10; i > 0; i--) {
    h = dht.readHumidity();
    if (!isnan(h)) break;
    delay(500);
  }

  if (isnan(h)) {
    Serial.println("Failed to read humidity");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println("%\t");
  }

  float t = 0;
  for (int i = 10; i > 0; i--) {
    t = dht.readTemperature();
    if (!isnan(t)) break;
    delay(500);
  }
   
  if (isnan(t)) {
    Serial.println("Failed to read temperature");
  } else {
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println("°C");
  }

  String temperature = String(t);
  String humidity = String(h);
  
  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}

void initWiFi()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  WiFi.config(staticIP, gateway, subnet);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" ");
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconnect");
      WiFi.begin(WLAN_SSID, WLAN_PASS);
      WiFi.config(staticIP, gateway, subnet);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println(" ");
      Serial.print("Connected. IP address: ");
      Serial.println(WiFi.localIP());
    }
    
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  initWiFi();
  
  dht.begin();
  delay(100);
  lastMeasured = 0;

  client.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);

  Serial.println("Device started");
  
//  readTemperatureAndHumidity();
//  ESP.deepSleep(10000000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  if (millis() - lastMeasured > 10000) {
    Serial.println("Read DHT values");
    readTemperatureAndHumidity();
    lastMeasured = millis();
  }

  client.loop();
}
