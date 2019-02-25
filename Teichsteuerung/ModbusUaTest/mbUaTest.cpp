#include <stdio.h>
#include <iostream>
#include <memory>
#include <sstream>
#include "MModbusTcpClient.h"

extern "C"
{
  #include "UAServer.h"
}

int main (int argc, char *argv[]) {
  std::unique_ptr<TMModbusTcpClient> ModbusClient;
  try
  {
    std::string ip = "192.168.179.199";
    int port = 502;
    uint16_t area = 4;
    uint16_t address = 2;
    int bitlen = 16;
    
    // UAServer starten
    const char * nodeId = "Seconds";
    int seconds=0;
    UAServer_start();
    UAServer_addIntVariable(nodeId, nodeId, nodeId, "Sekundenzaehler", seconds, 32);
    
    while (1)
    {
      try {
        usleep(100000);
        if (!ModbusClient) {
          // Modbus initialisieren und starten
          ModbusClient.reset(new TMModbusTcpClient(ip,port,0));
          if (!ModbusClient->Connect())
          {
            ModbusClient.reset();
            throw Exception("Verbindungsaufbau fehlgeschlagen");
          }
          ModbusClient->ReadRequest(area,address,bitlen);
        }
        if (ModbusClient) {
          int bytelen = (bitlen+7)/8;
          std::unique_ptr<uint8_t[]> buffer(new uint8_t[bytelen]);
          int16_t result = ModbusClient->ReadResponse(area,address,bitlen,buffer.get());
          if (result==0) {
            // Puffer auslesen und darstellen
            std::ostringstream str;
            for (int i = 0; i < bytelen; i++) {
              str << std::hex << "0x" << (int)buffer[i] << " ,";
            }
            if (bytelen>2) std::cout << "Buffer: " << str.str() << std::endl;
            if (bytelen>=2) {
              // Zahlenwert des ersten Registers ausgeben
              int value = (buffer[0]<<8 | (buffer[1]));
              if (value!=seconds)
              {
                std::cout << "Register1 changed: " << value << std::endl;
                UA_Server_writeIntVariable(nodeId, value);
                seconds = value;
              }  
            }
            // Daten erneut anfordern
            ModbusClient->ReadRequest(area,address,bitlen);
          }
          else if (result>0)
          {
            UA_Server_setVariableBad(nodeId);
            std::string msg = ModbusClient->GetErrorText(result);
            ModbusClient.reset();
            throw TMException(msg);
          }
        }
      }
      catch (Exception &ex)
      {
        UA_Server_setVariableBad(nodeId);
        std::cerr << ex.Message << std::endl;
      }
      // UAServer bearbeiten
      UAServer_loop();
    }  
  }
  catch (Exception &ex)
  {
    std::cerr << ex.Message << std::endl;
  }
}  

