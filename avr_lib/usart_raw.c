#include "usart_raw.h"

volatile uint8_t usart_buffercounter = 0;

uint8_t usart_rx_buffer[BUFFER_SIZE];
uint8_t *usart_rx_buffer_pointer_in	= &usart_rx_buffer[0];
uint8_t *usart_rx_buffer_pointer_out = &usart_rx_buffer[0];
	
//----------------------------------------------------------------------------
//Init serielle Schnittstelle
void usart_init(unsigned long baudrate) 
{ 
	//Serielle Schnittstelle 1
  	//Enable TXEN im Register UCR TX-Data Enable
	UCR =(1 << TXEN | 1 << RXEN | 1<< RXCIE);
	// 0 = Parity Mode Disabled
	// 1 = Parity Mode Enabled, Even Parity
	// 2 = Parity Mode Enabled, Odd Parity
	//UCSRC = 0x06 + ((parity+1)<<4);
	//UCSRC |= (1<<USBS);
	//Teiler wird gesetzt 
	UBRR=(F_CPU / (baudrate * 16L) - 1);
}

//----------------------------------------------------------------------------
//Routine für das schreiben eines Bytes
void usart_write(uint8_t b)
{
	//Warten solange bis Zeichen gesendet wurde
	while(!(USR & (1<<UDRE)));
	//Ausgabe des Zeichens
	UDR = b;
}

//Routine für das schreiben eines Puffers
void usart_write_buffer(const uint8_t *buffer, uint8_t len)
{
	while (len-- > 0 ) usart_write(*buffer++);
}

//----------------------------------------------------------------------------
//Routine für das Lesen eines Bytes
uint8_t usart_read()
{
	uint8_t b = 0;
	if(usart_buffercounter>0)
	{
		// Gelesen wird über den Ausgabepointer
		b = *usart_rx_buffer_pointer_out++;
		usart_buffercounter--;
		if (usart_rx_buffer_pointer_out >= usart_rx_buffer+BUFFER_SIZE)
		{
			// Ringpuffer wieder von vorn beginnen
			usart_rx_buffer_pointer_out = usart_rx_buffer;
		}
	}
	return b;
}

uint8_t usart_read_buffer(uint8_t *buffer, uint8_t maxlen)
{
	uint8_t len=0;
	while (usart_buffercounter>0 && len<maxlen)
	{
		*buffer++ = usart_read();
		len++;
	}
	return len;
}

//----------------------------------------------------------------------------
//Empfang eines Zeichens
ISR (USART_RX)
{
	unsigned char receive_char;
	receive_char = (UDR);
		
	if (usart_buffercounter <= BUFFER_SIZE)
	{
		// Das gelesene Zeichen kommt über den Eingabepointer in den Puffer
		*usart_rx_buffer_pointer_in++ = receive_char;
		usart_buffercounter++;    
		if (usart_rx_buffer_pointer_in >= usart_rx_buffer+BUFFER_SIZE)
		{
			// Ringpuffer wieder von vorn beginnen
			usart_rx_buffer_pointer_in = usart_rx_buffer;
		}
	}
}



