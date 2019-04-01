#include "ModbusTCPSlave.h"
#include "temperature.h"
#include "teranis.h" 

ModbusTCPSlave modbusTCPSlave; 

void setup(void)
{
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

  setup_temperature();
  setup_wifi();
  setup_ota();
  setup_telnet();

  modbusTCPSlave.begin();
  
  // Ausgänge konfigurieren
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
  setup_teranis();

  // Defaultwerte für Parameter
  R(int16_t,0) = 60; // Minimale Zeit für alle Pumpenaktionen
  R(int16_t,2) = 600; // Zeit bis der Min-Sensor im Filtergraben kommen muß wenn die Pumpe abgeschaltet wird
  R(int16_t,4) = 6000; // Maximale Stillstandszeit der Pumpe
  R(int16_t,6) = 600; // Maximale Laufzeit der Pumpe
  R(int16_t,8) = 220; // Teichtemperatur ab der auf kühlen geschaltet wird
  R(int16_t,10) = 180; // Teichtemperatur ab der auf heizen geschaltet wird
}

uint8_t oldi = 0;
uint8_t oldq = 0;

void ChangePrint(uint8_t newval, uint8_t &oldval, const char *text)
{
  if (newval!=oldval)
  {
    oldval = newval;
    char msg[50];
    snprintf (msg, 49, "%ld: %s=%X",millis(),text,oldval);
    TelnetMsg(msg);
    Serial.println(msg);
  }
}

void loop(void)
{
  if (loop_ota()) return;

  // now übertragen
  I(int64_t,20) = millis();

  IX(0,0)=digitalRead(D1); // Pumpe MIN
  IX(0,1)=digitalRead(D2); // Filter MIN

  loop_teranis();

  digitalWrite(D8,QX(0,0)); // Pumpe
  if (QX(0,1)) read_temperatures();
  digitalWrite(D7,QX(0,2)); // Frischwasser
  digitalWrite(D6,QX(0,6)); // Temperaturen ein

  ChangePrint(Inputs[0],oldi,"Input0");
  ChangePrint(Outputs[0],oldq,"Output0");

  modbusTCPSlave.Run();
  Telnet();
}
