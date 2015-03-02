#1/usr/bin/python
#
# import pymodbus
# import serial
# from pymodbus.pdu import ModbusRequest
import time

# initialize a serial RTU client instance
from pymodbus.client.sync import ModbusSerialClient as ModbusClient
# from pymodbus.transaction import ModbusRtuFramer

# import logging
# logging.basicConfig()
# log = logging.getLogger()
# log.setLevel(logging.DEBUG)

# count= the number of registers to read
# unit= the slave unit this request is targeting
# address= the starting address to read from

client = ModbusClient(method="rtu", port="/dev/ttyUSB0", stopbits=1, bytesize=8, parity='N', baudrate= 9600,
                      timeout=0.018)

# connect to the serial modbus server
connection = client.connect()
print connection

slave = 0x01

t = time.time()
# starting add, num of reg to read, slave unit.
print('lese')
result= client.read_holding_registers(0x00, 1, unit=slave)
print(time.time() - t)
print('schreibe')
result = client.write_register(0x00, 1 if result.registers[0] == 0 else 0, unit=slave)
print(time.time() - t)
print('fertich')

# print(result.registers[0])

# closes the underlying socket connection
client.close()
print(time.time() - t)
