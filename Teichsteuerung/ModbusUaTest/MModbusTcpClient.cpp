//---------------------------------------------------------------------------
#include "MModbusTcpClient.h"
#include <time.h>

/***

Der Modbus-Standard hat 4 Adressbereiche.
Dafür werden die folgenden Funktionscodes verwendet.

0x Binäre Ausgänge (Coils) (1-bit Wert)
Adressbereich 00001 bis 09999 => z.B. 0x0001 bis 0x9999
    Funktionscodes
    1 – Read Coil Status
    5 – Write Coil Status

1x Binäre Eingänge (1-bit Wert)
Adressbereich 10001 bis 19999 => z.B. 1x0001 bis 1x9999
    Funktionscode
    2 – Read Input Status

3x Analoge Eingänge - Input-Register (16-bit Wert)
Adressbereich 30001 bis 39999  => z.B. 3x0001 .. 3x9999
    Funktionscode
    4 – Read Input Register

4x Analoge Ausgänge - Holding-Register (16-bit Wert)
Adressbereich 40001 bis 49999 => z.B. 4x0001 bis 4x9999
    Funktionscodes
    3 – Read Holding Register
    6 – Write Holding Register
    0x10 – Write Multiple Registers

***/

TMModbusTcpClient::TSeq::TSeq()
{
   this->seq = 0;
}
//---------------------------------------------------------------------------
uint16_t TMModbusTcpClient::TSeq::GetSeq()
{
  if(++this->seq>0xFF)this->seq = 0;
  return seq;
}
//---------------------------------------------------------------------------
TMModbusTcpClient::TRtuPdu::TRtuPdu(uint8_t modbusadr, uint8_t function_code, uint16_t address, uint16_t value)
{
  this->modbusadr     = modbusadr;
  this->function_code = function_code;
  this->address       = Endian_swap(address);
  this->value         = Endian_swap(value);
}
//---------------------------------------------------------------------------
TMModbusTcpClient::TPdu::TPdu(uint16_t transaction_identifier, uint8_t modbusadr, uint8_t function_code, uint16_t address, uint16_t value)
{
  this->transaction_identifier = Endian_swap(transaction_identifier);
  this->protocol_identifier    = 0x0; // immer 0
  this->length                 = Endian_swap(0x6); // Länge der restlichen Daten
  this->modbusadr              = modbusadr;
  this->function_code          = function_code;
  this->address                = Endian_swap(address);
  this->value                  = Endian_swap(value);
}
//---------------------------------------------------------------------------
void TMModbusTcpClient::ReadRequest(uint16_t area, uint16_t address, int bitlen)
{
  // Empfangspuffer leeren
  readCount = 0;
  for(int i=0;i<BUFFER_SIZE;i++) readBuffer[i]=0;
  if(!socket.IsReady()) throw Exception("Socket ist geschlossen");
  uint8_t function_code=0;
  int len = 0;
  switch (area)
  {
    case 0 :
        function_code = 1; // Read Coil Status
        len = bitlen;
        break;
    case 1 : function_code = 2; // Read Input Status
        len = bitlen;
        break;
    case 3 : function_code = 4; // Read Input Register
        len = (bitlen+15)/16; // in Worte aufgerundet umrechnen
        break;
    case 4 : function_code = 3; // Read Holding Register
        len = (bitlen+15)/16; // in Worte aufgerundet umrechnen
        break;
    default: throw TMException("Bereich $ kann über Modbus nicht gelesen werden.",area);
  }
  socket.Flush(); // Empfang leeren
  // PDU zusammenstellen
  TPdu pdu(seq.GetSeq(), modbusadr, function_code, address, len);
  // Telegramm absenden
  socket.Send(pdu.GetPdu(),pdu.GetSize());
  requestTime = clock();
}
//---------------------------------------------------------------------------
int16_t TMModbusTcpClient::ReadResponse(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer)
{
  if(!socket.IsReady()) throw TMException("Schnittstellenfehler");

  if ((clock()-requestTime)/CLOCKS_PER_SEC > 5) throw TMException("Zeitüberschreitung");

  // Empfangen
  int rc = socket.Recv((char*)readBuffer+readCount,BUFFER_SIZE-readCount);
  if(rc==-1) throw TMException("Schnittstellenfehler"); // socket error
  if(rc==0) return -1; // nichts gelesen

  readCount += rc;
  if(readCount >= 9)
  {
    if (readBuffer[7] & 0x80) {
      // Fehlerrückmeldung gelesen
      throw TMException(GetErrorText(readBuffer[8]));
    }
    // Modbus Header gelesen
    uint8_t byteCount = readBuffer[8];
    if (byteCount>(255-9)) {
      throw TMException("empfangene Telegrammlänge zu groß.");
    }
    if (readBuffer[0]!=0 || readBuffer[2]!=0 || readBuffer[3]!=0
      || readBuffer[4]!=0 || readBuffer[5]!=byteCount+3u) throw TMException("unerwartete Antwort");
    if (readCount==byteCount+9)
    {
      // Daten komplett gelesen
      // Daten in den Empfangspuffer kopieren
      int bytelen = (bitlen+7)/8;
      if (byteCount>bytelen) {
        // Größe begrenzen, es kann sein, dass ein Byte mehr ankommt
        byteCount = bytelen;
      }
      for(int i=0;i<byteCount;i++)
      {
        buffer[i] = readBuffer[i+9];
      }
      return 0; // fertig gelesen
    }
  }
  return -1; // sonst noch unvollständig
}
//---------------------------------------------------------------------------
void TMModbusTcpClient::WriteRequest(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer)
{
  if(!socket.IsReady()) throw TMException("Modbus Fehler beim Schreiben, Socket ist geschlossen.");
  uint8_t function_code=0;
  int value=0;
  switch (area)
  {
    case 0 : function_code = 5; // Write Coil Status
        if (bitlen!=1) throw TMException("Über Modbus kann in Bereich $ nur ein einzelnes Bit geschrieben werden.",area);
        value = buffer[0] & 1;
        break;
    case 4 : function_code = 6; // Write Holding Register
        if (bitlen!=16) throw TMException("Über Modbus kann in Bereich $ nur ein einzelnes Wort geschrieben werden.",area);
        value = buffer[0]*256 + buffer[1]; // TODO: nochmal prüfen ob das richtig herum ist.
        break;
    default:throw TMException("Bereich $ kann über Modbus nicht geschrieben werden.",area);
  }
  socket.Flush(); // Empfang leeren
  TPdu pdu(seq.GetSeq(), modbusadr, function_code, address, value);
  socket.Send(pdu.GetPdu(),pdu.GetSize());
}
//---------------------------------------------------------------------------
int16_t TMModbusTcpClient::WriteResponse()
{
  if(!socket.IsReady()) throw TMException("Schnittstellenfehler");

  if ((clock()-requestTime)/CLOCKS_PER_SEC > 5) throw TMException("Zeitüberschreitung");

  // Empfangen
  int rc = socket.Recv((char*)readBuffer+readCount,BUFFER_SIZE-readCount);
  if(rc==-1) throw TMException("Schnittstellenfehler"); // socket error
  if(rc==0) return -1; // nichts gelesen

  readCount += rc;
  if(readCount >= 9)
  {
    if (readBuffer[7] & 0x80) {
      // Fehlerrückmeldung gelesen
      throw TMException(GetErrorText(readBuffer[8]));
    }
    // Modbus Header gelesen
    if (readBuffer[0]!=0 || readBuffer[2]!=0 || readBuffer[3]!=0
      || readBuffer[4]!=0 || readBuffer[5]!=6) throw TMException("unerwartete Antwort");
    return 0; // fertig gelesen
  }
  return -1; // sonst noch unvollständig
}
//---------------------------------------------------------------------------
bool TMModbusTcpClient::Connect()
{
  if(socket.IsReady()) return true;
  return socket.Connect(port,ip);
}
//---------------------------------------------------------------------------
void TMModbusTcpClient::Disconnect()
{
  socket.Disconnect();
}
//---------------------------------------------------------------------------

TMModbusTcpClient::TMModbusTcpClient (STRING ip, int port, uint8_t modbusadr)
: TMModbusClient(modbusadr)
{
  this->ip = ip;
  this->port = port;
}

TMModbusTcpClient::~TMModbusTcpClient()
{
  Disconnect();
}

