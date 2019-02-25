/*
 * ursprünglicher Code von https://github.com/JhonControl/Arduino_ESP8266_Modbus_Industrial_Applications
 * stark überarbeitet
*/
#include <Arduino.h>
#include "ModbusTCPSlave.h"

// Register kommen von TERANiS
extern "C"{ 
  #include "teranis.h" 
};

#define MB_PORT 502  

//#define MBDebug     //serial debug enable für Arduino

//
// MODBUS Function Codes
//
#define MB_FC_NONE 0
#define MB_FC_READ_COILS 1
#define MB_FC_READ_DISCRETE_INPUT 2
#define MB_FC_READ_REGISTERS 3              //implemented
#define MB_FC_READ_INPUT_REGISTERS 4        //implemented
#define MB_FC_WRITE_COIL 5
#define MB_FC_WRITE_REGISTER 6              //implemented
#define MB_FC_WRITE_MULTIPLE_COILS 15
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16   //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 6
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID          0
#define MB_TCP_PID          2
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7
#define MB_TCP_REGISTER_START   8
#define MB_TCP_REGISTER_NUMBER  10


#define MAX_MODBUS_CLIENTS 2
WiFiServer MBServer(MB_PORT);
WiFiClient MBClient[MAX_MODBUS_CLIENTS];

void ModbusTCPSlave::begin(){
  MBServer.begin();
  #ifdef MBDebug
    Serial.print(F("Listening on "));
    Serial.print(MB_PORT);
    Serial.println(" ...");
  #endif
}

void ModbusTCPSlave::Run(){
  // Cleanup disconnected session
  for(uint8_t i = 0; i < MAX_MODBUS_CLIENTS; i++)
  {
    if (MBClient[i] && !MBClient[i].connected())
    {
      Serial.print("Modbus Client disconnected ... terminate session "); Serial.println(i+1); 
      MBClient[i].stop();
    }
  }
  
  // Check new Modbus Client connections
  if (MBServer.hasClient())
  {
    bool ConnectionEstablished = false; // Set to false
    
    for(uint8_t i = 0; i < MAX_MODBUS_CLIENTS; i++)
    {
      // find free socket
      if (!MBClient[i])
      {
        MBClient[i] = MBServer.available(); 
        ConnectionEstablished = true; 
        Serial.print("Modbus new Client connected."); Serial.println(i+1); 
        break;
      }
      else
      {
        // Serial.println("Session is in use");
      }
    }
 
    if (ConnectionEstablished == false)
    {
      Serial.println("No free sessions ... drop connection");
      MBServer.available().stop();
    }
  }
 
  for(uint8_t i = 0; i < MAX_MODBUS_CLIENTS; i++)
  {
    if (MBClient[i] && MBClient[i].connected())
    {
      if(MBClient[i].available())
      { 
        Receive(MBClient[i]);
      }
    }
  }
}
  
void ModbusTCPSlave::Receive(WiFiClient &client)
{  
  byte byteFN = MB_FC_NONE;
  byte byteEC = MB_EC_NONE;
  int Start;
  int WordDataLength;
  int ByteDataLength;
  int MessageLength;
  byte ByteArray[260];
  int i = 0;
  while(client.available() && i<(sizeof(ByteArray)/sizeof(*ByteArray)))
  {
    ByteArray[i] = client.read();
    i++;
  }
  client.flush();
  byteFN = ByteArray[MB_TCP_FUNC];
  Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
  WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
  #ifdef MBDebug
    Serial.println();
    Serial.print("RX: ");
    for (byte thisByte = 0; thisByte < 20; thisByte++) {
      Serial.print(ByteArray[thisByte], DEC);
      Serial.print("-");
    }
    Serial.println();
    Serial.print("Funktionscode: ");
    Serial.println(byteFN);
  #endif
  // Handle request

  switch(byteFN) {
      case MB_FC_READ_REGISTERS:  // 03 Read Holding Registers
          ByteDataLength = WordDataLength * 2;
          ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
          ByteArray[8] = ByteDataLength;     //Number of bytes after this one (or number of bytes of data).
          if (((Start+WordDataLength)*2)<PLC_F_SIZE)
          {
            for(int i = 0; i < WordDataLength; i++)
            {
              ByteArray[ 9 + i * 2] = Flags[(Start + i)*2 +1];
              ByteArray[10 + i * 2] = Flags[(Start + i)*2];
            }
            MessageLength = ByteDataLength + 9;
            client.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
            #endif
          }  
          else 
          {
            // Fehlercode zurückmelden
            byteEC = MB_EC_ILLEGAL_DATA_ADDRESS;
          }
          break;
          
      case MB_FC_READ_INPUT_REGISTERS:  // 04 Read Input Registers
          Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
          WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
          ByteDataLength = WordDataLength * 2;
          ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
          ByteArray[8] = ByteDataLength;     //Number of bytes after this one (or number of bytes of data).
          if (((Start+WordDataLength))*2<PLC_I_SIZE)
          {
            for(int i = 0; i < WordDataLength; i++)
            {
              ByteArray[ 9 + i * 2] = Inputs[(Start + i)*2 +1];
              ByteArray[10 + i * 2] = Inputs[(Start + i)*2];
            }
            MessageLength = ByteDataLength + 9;
            client.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
            #endif
          }  
          else 
          {
            // Fehlercode zurückmelden
            byteEC = MB_EC_ILLEGAL_DATA_ADDRESS;
          }
          break;
          
      case MB_FC_WRITE_REGISTER:  // 06 Write Holding Register
          if ((Start*2+1)<PLC_F_SIZE)
          {
            FREF(uint16_t,Start*2) = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
            ByteArray[5] = 6; //Number of bytes after this one.
            MessageLength = 12;
            client.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
                Serial.print("Write Holding Register: ");
                Serial.print(Start);
                Serial.print("=");
                Serial.println((Start*2+1<PLC_F_SIZE) ? FREF(uint16_t,Start*2) : -1);
            #endif
          }
          else 
          {
            // Fehlercode zurückmelden
            byteEC = MB_EC_ILLEGAL_DATA_ADDRESS;
          }
          break;
          
      case MB_FC_WRITE_MULTIPLE_REGISTERS:    //16 Write Holding Registers
          ByteDataLength = WordDataLength * 2;
          ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
          if (((Start+WordDataLength)*2)<PLC_F_SIZE)
          {
            for(int i = 0; i < WordDataLength; i++)
            {
              FREF(uint16_t,(Start+i)*2) =  word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
            }
            MessageLength = 12;
            client.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
                Serial.print("Write Holding Registers from: ");
                Serial.print(Start);
                Serial.print("=");
                Serial.println(WordDataLength);
            #endif
          }  
          else 
          {
            // Fehlercode zurückmelden
            byteEC = MB_EC_ILLEGAL_DATA_ADDRESS;
          }
          break;
      default:
         // ungültiger Funktionscode
         byteEC = MB_EC_ILLEGAL_FUNCTION;     
  }
  if (byteEC != MB_EC_NONE)
  {
    #ifdef MBDebug
      Serial.print("Modbus Fehler: ");
      Serial.print(byteEC);
      Serial.print(" von ");
      Serial.print(Start);
      Serial.print(" bis ");
      Serial.println(WordDataLength);
    #endif
    ByteArray[MB_TCP_FUNC] |= 0x80; // Im Code Bit 7 setzen
    ByteArray[MB_TCP_REGISTER_START] |= byteEC; // Fehlercode speichern
    ByteArray[5] = 4; // Datenbytes nach diesem Byte
    MessageLength = MB_TCP_REGISTER_START+1;
    client.write((const uint8_t *)ByteArray,MessageLength);
  }
}
