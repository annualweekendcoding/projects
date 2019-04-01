//#include <Base64.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "teranis.h"

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

// function to print a device address
void printAddress(DeviceAddress &deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void setup_temperature() {
  Serial.println("Dallas Temperatures");

  // locate devices on the bus
  Serial.print("Locating devices...");

  // Start up the library
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Loop through each device, print out address
  numberOfRealDevices = 0;
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      numberOfRealDevices++;
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();

      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);

      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

// Adressen der Temperatursensoren am Teich
DeviceAddress Teich_vorn_oben_Temp_Adress = { 0x28, 0x88, 0x42, 0x2D, 0x01, 0x00, 0x00, 0x79 };
DeviceAddress Teich_vorn_unten_Temp_Adress = { 0x28, 0x3E, 0x66, 0x2D, 0x01, 0x00, 0x00, 0x2B };
DeviceAddress Solar_Ausgang_Temp_Adress = { 0x28, 0xE0, 0x70, 0x15, 0x02, 0x00, 0x00, 0xAC };
DeviceAddress Teich_Luft_Temp_Adress = { 0x28, 0x60, 0x1F, 0x2D, 0x01, 0x00, 0x00, 0x88 };
DeviceAddress Pumpe_Temp_Adress = { 0x28, 0x6c, 0x62, 0x2d, 0x01, 0x00, 0x00, 0x74 };
DeviceAddress Filter_Temp_Adress = { 0x28, 0x3b, 0x3a, 0x2d, 0x01, 0x00, 0x00, 0x74 };

// Funktion aus dem Reiter telnet
void TelnetMsg(String text);

void read_temperatures()
{
  if (numberOfRealDevices==0) 
  {
    setup_temperature();  
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
      char *ort="unbekannt";
      if (isEqual(tempDeviceAddress,Teich_vorn_oben_Temp_Adress)) 
      {
        ort="Teich_vorn_oben";
        I(int16_t,6)=tempC*10; // Zuweisung im TERANiS-Bereich
      }
      else if (isEqual(tempDeviceAddress,Teich_vorn_unten_Temp_Adress)) 
      {
        ort="Teich_vorn_unten";
        I(int16_t,8)=tempC*10; // Zuweisung im TERANiS-Bereich
      }
      else if (isEqual(tempDeviceAddress,Solar_Ausgang_Temp_Adress)) 
      {
        ort="Solar_Ausgang";
        I(int16_t,10)=tempC*10; // Zuweisung im TERANiS-Bereich
      }
      else if (isEqual(tempDeviceAddress,Teich_Luft_Temp_Adress)) 
      {
        ort="Luft";
        I(int16_t,12)=tempC*10; // Zuweisung im TERANiS-Bereich
      }
      else if (isEqual(tempDeviceAddress,Pumpe_Temp_Adress)) 
      {
        ort="Pumpe";
        I(int16_t,14)=tempC*10; // Zuweisung im TERANiS-Bereich
      }
      else if (isEqual(tempDeviceAddress,Filter_Temp_Adress)) 
      {
        ort="Filter";
        I(int16_t,16)=tempC*10; // Zuweisung im TERANiS-Bereich
      }
      else
      {
        // unbekanntes Gerät
        char msg[100];
        snprintf (msg, 100, "unbekannter Sensor: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
          tempDeviceAddress[0],tempDeviceAddress[1],tempDeviceAddress[2],tempDeviceAddress[3],
          tempDeviceAddress[4],tempDeviceAddress[5],tempDeviceAddress[6],tempDeviceAddress[7]);
        TelnetMsg(msg);
        Serial.println(msg);
      }
      // build Telnet Message
      char msg[50];
      snprintf (msg, 50, "Sensor: %i %s %f °C",i,ort,tempC);
      TelnetMsg(msg);
      Serial.println(msg);
    }
    //else ghost device! Check your power requirements and cabling
  }
}
