#!/usr/bin/python

class ModbusRtuClient:
    '''
    einfache Klasse fuer einen Modbus-RTU Client.
    Zur Initialisierung als socket sollte ein Serial-Objekt uebergeben werden
    '''


    def __init__(self, socket):
        self.__socket = socket
        self.open()

    def open(self):
        if not self.__socket.isOpen:self.self.__socket.open()

    def close(self):
        if self.__socket.isOpen:self.__socket.close()

    #CRC Calculation
    def __crccal(self, msg):
        #CRC (Cyclical Redundancy Check) Calculation
        crc = 0xFFFF
        crchi = 0xFF
        crclo = 0xFF
        crclsb = 0x00
        for i in range(0, len(msg)-2,+1):
            crc = (crc ^ msg[i])
            for j in range(0, 8):
                crclsb = (crc & 0x0001);
                crc = ((crc >> 1) & 0x7FFF)
                if (crclsb == 1):
                    crc = (crc ^ 0xA001)
        crchi = ((crc >> 8) & 0xFF)
        crclo = (crc & 0xFF)
        return (crclo,crchi) 

    #CRC Valdation
    def __crcvalid(self, resp):
        crc = self.__crccal(resp)
        if (crc[0]==resp[len(resp)-2]) & (crc[1]==resp[len(resp)-1]):return True
        return False

    #Modbus Function Code 16 = Preset Multiple Registers
    def preset_multiple_registers(self, slave,start,values):
        Slave_Address = slave
        Function = 16
        Starting_Address = start
        NumberofRegisters = len(values)
        Byte_Count = NumberofRegisters * 2
        message = [0 for i in range(9 + 2 * NumberofRegisters)]

        #index0 = Slave Address
        message[0] = (Slave_Address & 0xFF)
        #index1 = Function
        message[1] = (Function & 0xFF)
        #index2 = Starting Address Hi
        message[2] = ((Starting_Address >> 8) & 0xFF)
        #index3 = Starting Address Lo
        message[3] = (Starting_Address & 0xFF)
        #index4 = Number of Registers Hi
        message[4] = ((NumberofRegisters >> 8) & 0xFF)
        #index5 = Number of Registers Lo
        message[5] = (NumberofRegisters & 0xFF)
        #index6 = Byte Count
        message[6] = (Byte_Count & 0xFF)

        for i in range(0, NumberofRegisters):
            #Data Hi, index7 and index9
            message[7 + 2 * i] = ((values[i] >> 8) & 0xFF)
            #Data Lo, index8 and index10
            message[8 + 2 * i] = values[i] & 0xFF

        #CRC (Cyclical Redundancy Check) Calculation
        crc = self.__crccal(message)

        #index11= CRC Lo
        message[len(message) - 2] = crc[0] #CRCLo
        #index12 = CRC Hi
        message[len(message) - 1] = crc[1] #CRCHi

        if self.__socket.isOpen:
            self.__socket.write("".join(chr(h) for h in message))
            reading = self.__socket.read(8)
            response = [0 for i in range(len(reading))]
            for i in range(0, len(reading)):
                response[i] = ord(reading[i])

            if len(response)==8:
                crcok = self.__crcvalid(response)
                if crcok & (response[0]==slave) & (response[1]==Function):return True
        return False

    #Modbus Function Code 03 = Read Holding Registers
    def read_holding_registers(self, slave,start,NumOfPoints):
        #Function 3 request is always 8 bytes
        message = [0 for i in range(8)]
        Slave_Address = slave
        Function = 3
        Starting_Address = start
        Number_of_Points = NumOfPoints

        #index0 = Slave Address
        message[0] = Slave_Address
        #index1 = Function
        message[1] = Function
        #index2 = Starting Address Hi
        message[2] = ((Starting_Address >> 8)& 0xFF)
        #index3 = Starting Address Lo
        message[3] = (Starting_Address& 0xFF)
        #index4 = Number of Points Hi
        message[4] = ((Number_of_Points >> 8)& 0xFF)
        #index5 = Number of Points Lo
        message[5] = (Number_of_Points& 0xFF)

        #CRC (Cyclical Redundancy Check) Calculation
        crc = self.__crccal(message)

        #index6= CRC Lo
        message[len(message) - 2] = crc[0]#CRCLo
        #index7 = CRC Hi
        message[len(message) - 1] = crc[1]#CRCHi

        if self.__socket.isOpen:
            self.__socket.write("".join(chr(h) for h in message))
            responseFunc3total = 5 + 2 * Number_of_Points
            reading = self.__socket.read(responseFunc3total)
            response = [0 for i in range(len(reading))]
            for i in range(0, len(reading)):
                response[i] = ord(reading[i])

            if len(response)==responseFunc3total:
                crcok = self.__crcvalid(response)
                if crcok & (response[0]==slave) & (response[1]==Function):
                    #Byte Count in index 3 = responseFunc3[2]
                    #Number of Registers = byte count / 2 = responseFunc3[2] / 2
                    registers = ((response[2] / 2)& 0xFF)
                    values = [0 for i in range(registers)]
                    for i in range(0, len(values)):
                        #Data Hi and Registers1 from Index3
                        values[i] = response[2 * i + 3]
                        #Move to Hi
                        values[i] <<= 8
                        #Data Lo and Registers1 from Index4
                        values[i] += response[2 * i + 4]
                        negatif = values[i]>>15
                        if negatif==1:values[i]=values[i]*-1
                    return values
        return ()

