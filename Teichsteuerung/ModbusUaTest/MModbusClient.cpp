#include "MModbusClient.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
//---------------------------------------------------------------------------
STRING TMModbusClient::GetErrorText(uint8_t errorCode)
{
  switch(errorCode)
  {
    case 0 : return "kein Fehler";
    case 1 : return "Gerät erkennt den Funktionscode nicht";
    case 2 : return "unerlaubtes Speicherregister";
    case 3 : return "unerlaubter Wert oder Anzahl";
    case 6 : return "Gerät kann die Anfrage temporär nicht bearbeiten";
    case 0xB : return "keine Antwort vom adressierten Gerät";
    default : return "Reserved";
  }
}

TMModbusClient::TMModbusClient (uint8_t modbusadr)
{
  this->modbusadr = modbusadr;
  max_register_read = 120;
}

//---------------------------------------------------------------------------
void TMModbusClient::ReadRequestMultiple(uint16_t area, uint16_t address, int bitlen)
{
  elemCount = 0;
  int len = (bitlen+7)/8;
  // Auf maximale Registeranzahl begrenzen
  uint16_t sublen = MIN(max_register_read, len/2);
  // Request für einzelnen Registerbereich auslösen
  ReadRequest(area,address,sublen);
}
//---------------------------------------------------------------------------
int16_t TMModbusClient::ReadResponseMultiple(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer)
{
  int len = (bitlen+7)/8;
  uint16_t sublen = MIN(max_register_read, (len-elemCount)/2);
  int16_t result = ReadResponse(area,address+elemCount*2,sublen,buffer+elemCount);
  if (result!=0) return result;
  // ein Teil wurde empfangen
  elemCount+=sublen;
  if (elemCount < len)
  {
    // Aufruf wiederholen, wenn Pufferlänge die maximale Registeranzahl überschreitet
    sublen = MIN(max_register_read, (len-elemCount)/2);
    // Request für einzelnen Registerbereich auslösen
    ReadRequest(area,address+elemCount*2,sublen);
    return -1;
  }
  return 0; // ansonsten fertig melden
}



