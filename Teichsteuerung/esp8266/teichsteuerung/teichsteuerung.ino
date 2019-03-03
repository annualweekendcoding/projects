#include "ModbusTCPSlave.h"
#include "temperature.h"

extern "C"{ 
  #include "teranis.h" 
};

// Hilfsdefinitionen zum Zugriff auf TERANiS Variablen
//#define IINT(idx) I(int16_t,idx)
//#define FINT(idx) F(int16_t,idx)
//#define FDINT(idx) F(int32_t,idx)

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
}

void loop(void)
{
  if (loop_ota()) return;

  // now übertragen
  I(int64_t,20) = millis();

  loop_teranis();

  digitalWrite(D6,QX(0,6));
  if (QX(0,1)) read_temperatures();
  if (QX(0,1))
  {
    Serial.println(F(int16_t,4));
  }  
  modbusTCPSlave.Run();
  Telnet();
}
