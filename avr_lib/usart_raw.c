#include "usart_raw.h"

#if USART_RX_BUFFER_SIZE>0 
volatile uint8_t usart_rx_buffercounter = 0;
volatile uint8_t usart_rx_buffer[USART_RX_BUFFER_SIZE];
volatile uint8_t *usart_rx_buffer_pointer_in	= usart_rx_buffer;
volatile uint8_t *usart_rx_buffer_pointer_out = usart_rx_buffer;
#endif

#if USART_TX_BUFFER_SIZE>0
volatile uint8_t usart_tx_buffercounter = 0;
volatile uint8_t usart_tx_buffer[USART_TX_BUFFER_SIZE];
volatile uint8_t *usart_tx_buffer_pointer_in = usart_tx_buffer;
volatile uint8_t *usart_tx_buffer_pointer_out = usart_tx_buffer;
#endif

#ifdef USART_SAVE_LASTTIME
#include <timer.h>
volatile uint16_t usart_rx_lasttime;
#endif

//----------------------------------------------------------------------------
//Init serielle Schnittstelle
void usart_init(unsigned long baudrate)
{
  // Serielle Schnittstelle 1
  // Senden und Empfangen erlauben
  UCR = (1 << TXEN | 1 << RXEN 
#if USART_RX_BUFFER_SIZE>0
    | 1<< RXCIE // Interrupt nur mit Puffer einschalten
#endif
#if USART_TX_BUFFER_SIZE>0
    | 1<< TXCIE // Interrupt nur mit Puffer einschalten
#endif
  );
  // 0 = Parity Mode Disabled
  // 1 = Parity Mode Enabled, Even Parity
  // 2 = Parity Mode Enabled, Odd Parity
  //UCSRC = 0x06 + ((parity+1)<<4);
  //UCSRC |= (1<<USBS);
  //Teiler wird gesetzt
  UBRR=(F_CPU / (baudrate * 16L) - 1);
}

#if USART_TX_BUFFER_SIZE>0
// interne Routine sendet ein Zeichen aus dem Puffer, 
// wird für das erste Zeichen direkt und dann von der ISR aufgerufen
void usart_send_from_buffer()
{
  if (usart_tx_buffercounter)
  {
    usart_tx_buffercounter--;
    UDR = *usart_tx_buffer_pointer_out++; // nächstes Zeichen aus Puffer senden
    if (usart_tx_buffer_pointer_out >= usart_tx_buffer+USART_TX_BUFFER_SIZE)
    {
      // Ringpuffer wieder von vorn beginnen
      usart_tx_buffer_pointer_out = usart_tx_buffer;
    }
  }
}
#endif

//----------------------------------------------------------------------------
//Routine für das schreiben eines Bytes
uint8_t usart_write(uint8_t b)
{
#if USART_TX_BUFFER_SIZE>0
  // Code für das Senden über einen Sendepuffer  
  if (usart_tx_buffercounter>=USART_TX_BUFFER_SIZE) return 0; // Puffer ist voll

  *usart_tx_buffer_pointer_in++ = b;
  usart_tx_buffercounter++;
  if (usart_tx_buffer_pointer_in >= usart_tx_buffer+USART_TX_BUFFER_SIZE)
  {
    // Ringpuffer wieder von vorn beginnen
    usart_tx_buffer_pointer_in = usart_tx_buffer;
  }
  // Wenn das Senderegister leer ist, dann sofort das erste Zeichen senden
  // die folgenden werden dann über die ISR gesendet.
  if (USR & (1<<UDRE)) usart_send_from_buffer();
#else
  // ohne Puffer
  // Warten solange bis Senderegister leer wird
  while(!(USR & (1<<UDRE)));
	//Ausgabe des Zeichens
	UDR = b;
#endif
  return 0x80;  
}

//Routine für das schreiben eines Puffers
uint8_t usart_write_buffer(const uint8_t *buffer, uint8_t len)
{
  while (len-->0) 
  {
    uint8_t retval = usart_write(*buffer++);
    if (retval!=0x80) return len;
  }
  return len;  
}

//Routine für das schreiben eines Puffers aus dem Programmspeicher
uint8_t usart_write_buffer_p(const uint8_t *buffer, uint8_t len)
{
	while (len-->0)
	{
		uint8_t retval = usart_write(pgm_read_byte(buffer++));
		if (retval!=0x80) return len;
	}
	return len;
}

//----------------------------------------------------------------------------
//Routine für das Lesen eines Bytes
uint8_t usart_read()
{
#if USART_RX_BUFFER_SIZE>0
  uint8_t b = 0;
  if(usart_rx_buffercounter>0)
  {
    // Gelesen wird über den Ausgabepointer
    b = *usart_rx_buffer_pointer_out++;
    usart_rx_buffercounter--;
    if (usart_rx_buffer_pointer_out >= usart_rx_buffer+USART_RX_BUFFER_SIZE)
    {
      // Ringpuffer wieder von vorn beginnen
      usart_rx_buffer_pointer_out = usart_rx_buffer;
    }
  }
#else
  while ((USR & (1 << RXC)) == 0) {}; // Do nothing until data have been received and is ready to be read from UDR
  uint8_t b = UDR;
  #ifdef USART_SAVE_LASTTIME
    usart_rx_lasttime = time_ms;
  #endif
#endif
  return b;
}

uint8_t usart_read_buffer(uint8_t *buffer, uint8_t maxlen)
{
  uint8_t len=0;
  while (usart_rx_buffercounter>0 && len<maxlen)
  {
    *buffer++ = usart_read();
    len++;
  }
  return len;
}

void usart_flush()
{
  // USART FIFO komplett auslesen
  while ( USR & (1<<RXC) ) if(UDR);
#if USART_RX_BUFFER_SIZE>0
  // Empfangspuffer rücksetzen
  usart_rx_buffercounter = 0;
  usart_rx_buffer_pointer_in	= usart_rx_buffer;
  usart_rx_buffer_pointer_out = usart_rx_buffer;
#endif
}


#if USART_RX_BUFFER_SIZE>0
//----------------------------------------------------------------------------
//Interrupt Service Routine zum Empfang eines Zeichens
ISR (USART_RX)
{
  unsigned char receive_char;
  receive_char = (UDR);
  
  if (usart_rx_buffercounter <= USART_RX_BUFFER_SIZE)
  {
    // Das gelesene Zeichen kommt über den Eingabepointer in den Puffer
    *usart_rx_buffer_pointer_in++ = receive_char;
    usart_rx_buffercounter++;
    if (usart_rx_buffer_pointer_in >= usart_rx_buffer+USART_RX_BUFFER_SIZE)
    {
      // Ringpuffer wieder von vorn beginnen
      usart_rx_buffer_pointer_in = usart_rx_buffer;
    }
	#ifdef USART_SAVE_LASTTIME
	  // Zeit merken, diese kann dann durch Differenzbildung gemessen werden
	  usart_rx_lasttime = time_ms;
	#endif
  }
}
#endif

#if USART_TX_BUFFER_SIZE>0
// Interrupt Service Routine zum Senden eines Zeichens
ISR(USART_TX)
{
  usart_send_from_buffer();
}
#endif


