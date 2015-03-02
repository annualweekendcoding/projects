#ifndef _MODBUS_H_
#define _MODBUS_H_

// Die Konfiguration aus dem Projekt wird hier eingebunden
#include "modbus_config.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#define	MODBUS_MAX_BYTES		((uint8_t)(MODBUS_MAX_BITADDRESSES / 8) + ((MODBUS_MAX_BITADDRESSES % 8)?1:0))


/* Slave index */
#define HEADER_LENGTH_RTU           1
#define PRESET_QUERY_LENGTH_RTU     6
#define PRESET_RESPONSE_LENGTH_RTU  2

#define CHECKSUM_LENGTH_RTU         2
#define MODBUS_BROADCAST_ADDRESS  255


/* Function codes */
#define FC_READ_COIL_STATUS          0x01  /* discretes inputs */
#define FC_READ_INPUT_STATUS         0x02  /* discretes outputs */
#define FC_READ_HOLDING_REGISTERS    0x03
#define FC_READ_INPUT_REGISTERS      0x04
#define FC_FORCE_SINGLE_COIL         0x05
#define FC_PRESET_SINGLE_REGISTER    0x06
#define FC_READ_EXCEPTION_STATUS     0x07
#define FC_FORCE_MULTIPLE_COILS      0x0F
#define FC_PRESET_MULTIPLE_REGISTERS 0x10
#define FC_REPORT_SLAVE_ID           0x11

/* Protocol exceptions */
#define ILLEGAL_FUNCTION        -0x01
#define ILLEGAL_DATA_ADDRESS    -0x02
#define ILLEGAL_DATA_VALUE      -0x03
#define SLAVE_DEVICE_FAILURE    -0x04
#define SERVER_FAILURE          -0x04
#define ACKNOWLEDGE             -0x05
#define SLAVE_DEVICE_BUSY       -0x06
#define SERVER_BUSY             -0x06
#define NEGATIVE_ACKNOWLEDGE    -0x07
#define MEMORY_PARITY_ERROR     -0x08
#define GATEWAY_PROBLEM_PATH    -0x0A
#define GATEWAY_PROBLEM_TARGET  -0x0B

/* Local */
#define INVALID_DATA            -0x10
#define INVALID_CRC             -0x11
#define INVALID_EXCEPTION_CODE  -0x12

#define SELECT_TIMEOUT          -0x13
#define SELECT_FAILURE          -0x14
#define NOT_FOR_US	        	  -0x15
#define CONNECTION_CLOSED       -0x16
#define MB_EXCEPTION            -0x17


/* It's not really the minimal length (the real one is report slave ID
 * in RTU (4 bytes)) but it's a convenient size to use in RTU or TCP
 * communications to read many values or write a single one.
 * Maximum between :
 * - HEADER_LENGTH_TCP (7) + function (1) + address (2) + number (2)
 * - HEADER_LENGTH_RTU (1) + function (1) + address (2) + number (2) + CRC (2)
*/
#define MIN_QUERY_LENGTH           12

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5:
 *  - RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 *  - TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes
 */
// Das Telegramm kann nicht größer sein, als der zur Verfügung stehende Sendepuffer
// Da das maximal 254 Byte sind, wird hier direkt die Puffergröße übernommen
#define MAX_ADU_LENGTH_RTU        USART_TX_BUFFER_SIZE
#define MAX_PDU_LENGTH            MAX_ADU_LENGTH_RTU-3

#define EXCEPTION_RESPONSE_LENGTH_RTU  5

/* Internal using */
#define MSG_LENGTH_UNDEFINED -1

typedef struct {
	uint8_t		cBit;
	uint8_t		cVal;
} tMBChangedBit ;

// CRC-Fehlerzähler zur Ermittlung der Signalqualität
extern uint8_t modbus_crc_errors;

// Diese Funktion vom Zyklus aus aufrufen
void modbus_cycle();

// hier sind die Modbus-Register die vom Master gelesen und geschrieben werden
extern volatile uint16_t modbus_words[MODBUS_MAX_WORDADDRESSES];
extern volatile uint8_t modbus_bytes[MODBUS_MAX_BYTES];

#endif

