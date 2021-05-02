#include "ModbusTCPSlave.h"
#include "temperature.h"
#include "teranis.h" 

ModbusTCPSlave modbusTCPSlave; 

void setup(void)
{
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

  // Ausgänge konfigurieren
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
  digitalWrite(D6,1); // Temperaturen ein

  setup_temperature();
  setup_wifi();
  setup_ota();
  setup_telnet();

  modbusTCPSlave.begin();
  
  setup_teranis();

  // Defaultwerte für Parameter
  R(int16_t,0) = 60; // Minimale Zeit für alle Pumpenaktionen
  R(int16_t,2) = 600; // Zeit bis der Min-Sensor im Filtergraben kommen muß wenn die Pumpe abgeschaltet wird
  R(int16_t,4) = 6000; // Maximale Stillstandszeit der Pumpe
  R(int16_t,6) = 600; // Maximale Laufzeit der Pumpe
  R(int16_t,8) = 230; // Teichtemperatur ab der auf kühlen geschaltet wird
  R(int16_t,10) = 190; // Teichtemperatur ab der auf heizen geschaltet wird
}

void ChangePrint(uint16_t newval, uint16_t &oldval, const char *text)
{
  if (newval!=oldval)
  {
    oldval = newval;
    char msg[50];
    snprintf (msg, 49, "%ld: %s = %X %d",millis(),text,oldval,oldval);
    TelnetMsg(msg);
    Serial.println(msg);
  }
}

#define CP(X) { static uint16_t old; ChangePrint(X,old,#X); }

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

  CP(Inputs[0]);
  CP(Outputs[0]);
  CP(I(int16_t,12));

  modbusTCPSlave.Run();
  Telnet();
}
