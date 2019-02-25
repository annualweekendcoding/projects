#ifndef MModbusTcpClient_H
#define MModbusTcpClient_H

#include "MModbusClient.h"

#include "TCPSocket.h"

class TMModbusTcpClient : public TMModbusClient
{
	protected:
    class TSeq {
      // Bildet eine Sequenznummer ab
      private :
        uint16_t seq;

      public :
        TSeq();
        uint16_t GetSeq();
    };
    //---------------------------------------------------------------------------
    class TRtuPdu {
      // PDU für ein Telegramm an den Modbus Slave
      private :
        uint8_t modbusadr;
        uint8_t function_code;
        uint16_t address;
        uint16_t value;
      public :
        TRtuPdu(uint8_t modbusadr, uint8_t function_code, uint16_t address, uint16_t value);
        void* GetPdu() {return this;};
        int GetSize() {return sizeof(*this);};
    };
    class TPdu {
      // PDU für ein Telegramm an den Modbus Slave
      private :
        uint16_t transaction_identifier;
        uint16_t protocol_identifier;
        uint16_t length;
        uint8_t modbusadr;
        uint8_t function_code;
        uint16_t address;
        uint16_t value;
      public :
        TPdu(uint16_t transaction_identifier, uint8_t modbusadr, uint8_t function_code, uint16_t address, uint16_t value);
        void* GetPdu() {return this;};
        int GetSize() {return sizeof(*this);};
    };
		bool IsReadReq;
	private:
		bool zyklus_aktiv;
    std::string ip; // IP-Adresse des Partners
    int port; // TCP-Port Default=502
    uint8_t modbusadr; // Modbus-Teilnehmeradresse
    TSeq seq;
    TTCPSocket socket; // Socket für Modbus-TCP
    static const int BUFFER_SIZE = 264;
    uint8_t readBuffer[BUFFER_SIZE]; // Puffer für einzelnes Empfangstelegramm
    int readCount; // Anzahl Bytes die schon gelesen wurden
    clock_t requestTime; //
	public:
    virtual bool Connect();
    virtual void Disconnect();

    // aus area 0,1,3,4 lesen
    virtual void ReadRequest(uint16_t area, uint16_t address, int len);
    virtual int16_t ReadResponse(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer);
    // in area 0,4 schreiben
    virtual void WriteRequest(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer);
    virtual int16_t WriteResponse();
		TMModbusTcpClient (std::string ip, int port, uint8_t modbusadr);
    virtual ~TMModbusTcpClient();
};

#endif
