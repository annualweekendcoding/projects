#ifndef MModbusClient_H
#define MModbusClient_H

#include "MModbusBase.h"

class TMModbusClient : public TMModbusBase
{
	protected:
    uint8_t modbusadr; // Modbus-Teilnehmeradresse
    int max_register_read; // maximale Anzahl Register die mit einem Telegramm gelesen werden kann
  private:
    int elemCount; // Anzahl Register (in Bytes) die schon gelesen wurden
  public:
    virtual bool Connect() = 0;
    virtual void Disconnect() = 0;

    /// aus area 0,1,3,4 lesen
    /// ruft zusammen mit ReadResponseMultiple ggf. mehrfach ReadRequest und ReadResponse auf.
    virtual void ReadRequestMultiple(uint16_t area, uint16_t address, int bitlen);
    /// Antwort auf ReadRequestMultiple empfangen?
    /// @result: 0 erfolgreich emfangen
    ///            -1 nichts oder noch unvollständig empfangen
    ///            1..n Fehler beim Empfang oder vom Partner (Fehlercode mit getErrorText zu entschlüsseln)
    virtual int16_t ReadResponseMultiple(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer);

    // aus area 0,1,3,4 lesen
    virtual void ReadRequest(uint16_t area, uint16_t address, int bitlen) = 0;
    /// Antwort auf ReadRequest empfangen?
    /// @result: 0 erfolgreich emfangen
    ///            -1 nichts oder noch unvollständig empfangen
    ///            1..n Fehler beim Empfang oder vom Partner (Fehlercode mit getErrorText zu entschlüsseln)
    virtual int16_t ReadResponse(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer) = 0;
    /// in area 0,4 schreiben
    virtual void WriteRequest(uint16_t area, uint16_t address, int bitlen, uint8_t* buffer) = 0;
    virtual int16_t WriteResponse() = 0;
    virtual std::string GetErrorText(uint8_t errorCode);
		TMModbusClient (uint8_t modbusadr);
};

#endif
