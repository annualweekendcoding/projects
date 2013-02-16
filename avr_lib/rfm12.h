#ifndef _RFM12_H
#define _RFM12_H
/**** RFM 12 library for Atmel AVR Microcontrollers *******
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * @author Peter Fuhrmann, Hans-Gert Dahme, Soeren Heisrath
 */

/******************************************************
 *                                                    *
 *    NO  C O N F I G U R A T I O N  IN THIS FILE     *
 *                                                    *
 *      ( thou shall not change lines below )         *
 *                                                    *
 ******************************************************/

// use this file in your Project-Directory and set the include-path properly
#include "rfm12_config.h"

//states for the rx and tx buffers
#define RFM12_STATUS_FREE 0
#define RFM12_STATUS_OCCUPIED 1
#define RFM12_STATUS_COMPLETE 2

//states for the rx state machine
#define RFM12_STATUS_IDLE      0
#define RFM12_STATUS_RECEIVING 1

// possible return values for rfm12_tx() and
// rfm12_start_tx()
#define RFM12_TX_SUCCESS  0x00

#define RFM12_TX_OVERFLOW 0x01
#define RFM12_TX_ERROR    0x02
#define RFM12_TX_OCCUPIED 0x03

#define RFM12_TX_ENQUEUED 0x80


// RX ERROR-CODES
#define RFM12_RX_BUFFER_NOT_FREE  1
#define RFM12_RX_BUFFER_FULL      2
#define RFM12_RX_CRC_ERROR_HEADER 3
#define RFM12_RX_CRC_ERROR_DATA   4



//function protoypes
void rfm12_init();
//void rfm12_data(uint16_t d);

void rfm12_tick();

uint8_t rfm12_tx(uint8_t type, uint8_t *data, uint8_t len);

//static inline uint8_t rfm12_tx_status();

static inline uint8_t *rfm12_rx_buffer();
static inline void rfm12_rx_clear();

/* Private structs needed for inline functions */

// Header is len,type,crc,checksum
#define RFM12_HEADER_SIZE 4

#if RFM12_TX_BUFFER_SIZE>0
//Buffer and status for the message to be transmitted
struct rf_tx_buffer_t
{
	volatile uint8_t status;//is the buffer free or occupied?
	uint8_t num_bytes; 		//number of bytes to transmit (size in buffer)
	uint8_t bytecount;   //counter for the byte we are transmitting at the moment
	
	//we transmit the bytes beginning here
	uint8_t sync[2]; 		//sync bytes for receiver to start filling fifo
  #ifndef RFM12_RAW_MODE
	uint8_t len;			  //length byte - number of bytes in buffer
	uint8_t type;			  //type field for airlab
  uint8_t crc_data;   //crc checksum for data
	uint8_t crc_header;	//checksum over the former 3
  #endif
	uint8_t buffer[RFM12_TX_BUFFER_SIZE];//buffer for the raw bytes to be transmitted
} extern rf_tx_buffer;
#endif

#if RFM12_RX_BUFFER_SIZE>0
//for storing the received bytes.
struct rf_buffer_t
{
	volatile uint8_t status;//is the buffer free or is there complete data in it?
	uint8_t len;			  //length byte - number of bytes in buffer
  #ifndef RFM12_RAW_MODE
	uint8_t type;			  //type field for airlab
  uint8_t crc_data;   //crc checksum for data
	uint8_t crc_header;	//rx checksum
  #endif
	uint8_t buffer[RFM12_RX_BUFFER_SIZE]; //receive buffer
};


//buffer and status for the message to be received
struct rf_rx_buffer_t
{
	volatile uint8_t status;//are we idle, receiving or ignoring?
  #ifndef RFM12_RAW_MODE
	uint8_t num_bytes;		//number of bytes to be received
	uint8_t bytecount;		//received bytes counter
  #endif	
	//Buffers for storing incoming transmissions
	struct rf_buffer_t rf_buffers[2];
	
	//points to the rf_buffer in rf_buffers that will be filled next
	struct rf_buffer_t * rf_buffer_in;
	uint8_t buffer_in_num;
	
	//points to the rf_buffer in rf_buffers that will be returned for reading next
	struct rf_buffer_t * rf_buffer_out;
	uint8_t buffer_out_num;
} extern rf_rx_buffer;
#endif

void rfm12_set_wakeup_timer(uint16_t val);
void rfm12_powerDown();
uint8_t rfm12_lowPowerTx( uint8_t len, uint8_t type, uint8_t *data );


//inline function to return the rx buffer status byte
//(returns STATUS_FREE or STATUS_COMPLETE)
static inline uint8_t rfm12_rx_status()
{
#if RFM12_RX_BUFFER_SIZE>0
	return rf_rx_buffer.rf_buffer_out->status;
#else
  return RFM12_STATUS_OCCUPIED;
#endif
}

static inline uint8_t rfm12_rx_len()
{
#if RFM12_RX_BUFFER_SIZE>0
	return rf_rx_buffer.rf_buffer_out->len;
#else
  return 0;
#endif
}

static inline uint8_t rfm12_rx_type()
{
#if RFM12_RX_BUFFER_SIZE>0
  #ifndef RFM12_RAW_MODE
  	return rf_rx_buffer.rf_buffer_out->type;
  #else
    return 0;
  #endif
#else
  return 0;
#endif
}



//inline function to retrieve current rf buffer
#if RFM12_RX_BUFFER_SIZE>0
static inline uint8_t *rfm12_rx_buffer()
{
	return (uint8_t*) rf_rx_buffer.rf_buffer_out->buffer;
}
#endif



//inline function to clear buffer complete/occupied status
static inline void rfm12_rx_clear()
{
#if RFM12_RX_BUFFER_SIZE>0
	//mark the current buffer as empty
	rf_rx_buffer.rf_buffer_out->status = RFM12_STATUS_FREE;
	
	//switch to the other buffer
	rf_rx_buffer.buffer_out_num = (rf_rx_buffer.buffer_out_num + 1 ) % 2 ;
	rf_rx_buffer.rf_buffer_out = &rf_rx_buffer.rf_buffers[rf_rx_buffer.buffer_out_num];
#endif
}

#ifdef RFM12_RECEIVE_CW
#define RFM12_RFRXBUF_SIZE 55
#define RFM12_STATE_EMPTY 0
#define RFM12_STATE_RECEIVING 1
#define RFM12_STATE_FULL 2

typedef struct
{
  volatile	uint8_t p;
  volatile	uint8_t state;
	uint8_t buf[RFM12_RFRXBUF_SIZE];
} rfrxbuf_t;


extern rfrxbuf_t cw_rxbuf;
#endif

#if RFM12_UART_DEBUG > 0
  #include "usart.h"
  #define RFM12_DEBUGLEVEL RFM12_UART_DEBUG
  #define debug_write_char(c) usart_write_char(c)
  #define debug_write_str(s) usart_write_str(s)
  #define debug_write_Pstr(s) usart_write_P(s)
  #define debug_write_endl usart_write_P(PSTR("\r\n"))
  #define debug_write_str_long(s) debug_write_str(s,s)
  #define debug_printInt(i) usart_write_P(PSTR("%i"),i)
  #define debug_printHex(i) usart_write_P(PSTR("%x"),i)
#elif RFM12_LCD_DEBUG > 0
  #include "textlcd4.h"
  #define RFM12_DEBUGLEVEL RFM12_LCD_DEBUG
  #define debug_write_char(c) lcd_step_char(c)
  #define debug_write_str(s) lcd_step_print(s)
  #define debug_write_Pstr(s) lcd_step_printP(s)
  #define debug_write_endl ;
  #define debug_write_str_long(s) ;
  #define debug_printInt(i) lcd_step_printInt(i)
  #define debug_printHex(i) lcd_step_printHex(i)
#else
  #define debug_write_char(c) ;
  #define debug_write_str(s) ;
  #define debug_write_Pstr(s) ;
  #define debug_write_endl ;
  #define debug_write_str_long(s) ;
  #define debug_printInt(i) ;
  #define debug_printHex(i) ;
#endif

#endif

