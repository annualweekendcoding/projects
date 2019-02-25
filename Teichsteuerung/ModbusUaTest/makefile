###############################################################################
# Makefile for the project ModbusUaTestLinux
###############################################################################

## General Flags
PROJECT = MbUaTest
TARGET = $(PROJECT)
CC = gcc

## Options common to compile, link and assembly rules
COMMON =

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall
CFLAGS += -g
#CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=$(PROJECT).map

## Objects that must be built in order to link
OBJECTS = $(PROJECT).o open62541.o UAServer.o MModbusBase.o MModbusClient.o MModbusTcpClient.o MRegExp.o TCPSocket.o MSocket.o

LIBS =

## Objects explicitly added by the user
LINKONLYOBJECTS =

## Build
all: $(TARGET)

## Compile
$(PROJECT).o: $(PROJECT).cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
        
open62541.o: open62541.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

UAServer.o: UAServer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MModbusBase.o: MModbusBase.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MModbusClient.o: MModbusClient.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MModbusTcpClient.o: MModbusTcpClient.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MRegExp.o: MRegExp.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

TCPSocket.o: TCPSocket.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MSocket.o: MSocket.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET) -lstdc++

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(PROJECT).map
