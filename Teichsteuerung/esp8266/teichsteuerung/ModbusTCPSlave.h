/*
    ModbusTCPSlave.h - an Arduino library for a Modbus TCP slave.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * ursprünglicher Code von https://github.com/JhonControl/Arduino_ESP8266_Modbus_Industrial_Applications
 * stark überarbeitet
*/

// Note: The Arduino IDE does not respect conditional included
// header files in the main sketch so you have to select your
// here.
//
#ifndef ModbusTCPSlave_h
#define ModbusTCPSlave_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

class ModbusTCPSlave
{
  public:
    void begin();
    void Run();
    void Receive(WiFiClient &client);
};

#endif
