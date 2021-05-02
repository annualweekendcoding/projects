//#include <Base64.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "teranis.h"

// Funktion aus dem Reiter telnet
void TelnetMsg(String text);

// Data wire is plugged into port D5 on Wemos D1 mini
#define ONE_WIRE_BUS 14
#define TEMPERATURE_PRECISION 9 // Lower resolution
// conversion delay depends on resolution required
const int temperature_delay = 750 / (1 << (12 - TEMPERATURE_PRECISION));

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found
int numberOfRealDevices; // Number of real temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

// function to compare device addresses
bool isEqual(DeviceAddress &deviceAddress1, DeviceAddress &deviceAddress2)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress1[i] != deviceAddress2[i]) return false;
  }
  return true;
}

void setup_temperature() {
  Serial.println("Dallas Temperatures");

  // locate devices on the bus
  Serial.print("Locating devices...");

  // Start up the library
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  char msg[100];
  snprintf (msg, 100, "%d Geraete gefunden.",numberOfDevices);
  TelnetMsg(msg);
  Serial.println(msg);
  // report parasite power requirements
  snprintf (msg, 100, "Parasite Power State: %d.",sensors.isParasitePowerMode());
  TelnetMsg(msg);
  Serial.println(msg);

  // Loop through each device, print out address
  numberOfRealDevices = 0;
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      numberOfRealDevices++;
      snprintf (msg, 100, "Geraet %d gefunden, Adresse: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
          i,tempDeviceAddress[0],tempDeviceAddress[1],tempDeviceAddress[2],tempDeviceAddress[3],
          tempDeviceAddress[4],tempDeviceAddress[5],tempDeviceAddress[6],tempDeviceAddress[7]);
      TelnetMsg(msg);
      Serial.println(msg);

      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);

      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

      snprintf (msg, 100, "Resolution: ",sensors.getResolution(tempDeviceAddress));
      TelnetMsg(msg);
      Serial.println(msg);
    } else {
      snprintf (msg, 100, "Found ghost device at %d but could not detect address. Check power and cabling.",i);
      TelnetMsg(msg);
      Serial.println(msg);
    }
  }
}

// Adressen der Temperatursensoren am Teich
const int SensorCount = 6;
struct Sensor
{
  DeviceAddress address; // Adresse des Sensors
  float temp; // aktueller Temperaturwert
  char * ort; // Ort für Printausgabe
  int hmid; // Homematic ise_id
  float lastHmTemp; // letzter Temperaturwert an Homematic gesendet
  int64_t lastHmTime; // letzte Zeit an HM gesendet
} Sensors[SensorCount] =
{
  {{ 0x28, 0x88, 0x42, 0x2D, 0x01, 0x00, 0x00, 0x79 }, -50.0, "vorn oben", 16967, -50.0, 0 },
  {{ 0x28, 0x3E, 0x66, 0x2D, 0x01, 0x00, 0x00, 0x2B }, -50.0, "vorn unten", 16966, -50.0, 0 },
  {{ 0x28, 0xE0, 0x70, 0x15, 0x02, 0x00, 0x00, 0xAC }, -50.0, "Solar Ausgang", 16968, -50.0, 0 },
  {{ 0x28, 0x60, 0x1F, 0x2D, 0x01, 0x00, 0x00, 0x88 }, -50.0, "Luft", 16965, -50.0, 0 },
  {{ 0x28, 0x6c, 0x62, 0x2d, 0x01, 0x00, 0x00, 0x74 }, -50.0, "Pumpe", 16969, -50.0, 0 },
  {{ 0x28, 0x3b, 0x3a, 0x2d, 0x01, 0x00, 0x00, 0x74 }, -50.0, "Filter", 16964, -50.0, 0 }
};  


// Funktion aus dem Reiter Homematic
bool HMStateChange(int id, float value);

void read_temperatures()
{
  if (numberOfRealDevices<SensorCount) 
  {
    setup_temperature();  
  }
  // Defaultvalue für alle Sensoren setzen
  for (int i=0; i<SensorCount; i++)
  {
    Sensors[i].temp=-50.0;
  }   
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //sensors.requestTemperatures(); // Send the command to get temperatures
  // Loop through each device, print out temperature data
  numberOfRealDevices = 0;
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      numberOfRealDevices++;
      sensors.requestTemperaturesByAddress(tempDeviceAddress);
      float tempC = sensors.getTempC(tempDeviceAddress);
      bool found=false;
      for (int i=0; i<SensorCount; i++)
      {
        if (isEqual(tempDeviceAddress,Sensors[i].address)) 
        {
          Sensors[i].temp=tempC;
          found=true;
        }
      }  
      if (!found)
      {
        // unbekanntes Gerät
        char msg[100];
        snprintf (msg, 100, "unbekannter Sensor: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
          tempDeviceAddress[0],tempDeviceAddress[1],tempDeviceAddress[2],tempDeviceAddress[3],
          tempDeviceAddress[4],tempDeviceAddress[5],tempDeviceAddress[6],tempDeviceAddress[7]);
        TelnetMsg(msg);
        Serial.println(msg);
      }
    }
    //else ghost device! Check your power requirements and cabling
  }
  // Werte setzen
  int64_t akttime = millis();
  // TERANiS-Variablen setzen
  I(int16_t,6) = Sensors[0].temp*10;   
  I(int16_t,8) = Sensors[1].temp*10;   
  I(int16_t,10) = Sensors[2].temp*10;   
  I(int16_t,12) = Sensors[3].temp*10;   
  I(int16_t,14) = Sensors[4].temp*10;   
  I(int16_t,16) = Sensors[5].temp*10;   
  for (int i=0; i<SensorCount; i++)
  {
    // Homematic nur max alle 3min setzen
    if (Sensors[i].hmid!=0 && Sensors[i].temp!=Sensors[i].lastHmTemp && (akttime-Sensors[i].lastHmTime)>3*60*1000) 
    {
      if (HMStateChange(Sensors[i].hmid,Sensors[i].temp)) Sensors[i].lastHmTemp = Sensors[i].temp;
      Sensors[i].lastHmTime = akttime;
    }
    if (Sensors[i].temp>-50.0)
    {
      // build Telnet Message
      char msg[50];
      snprintf (msg, 50, "Sensor: %s %f °C",Sensors[i].ort,Sensors[i].temp);
      TelnetMsg(msg);
      Serial.println(msg);
    }
  }   
}
