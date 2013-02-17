#include "usart_raw.h"

volatile uint8_t usart_rx_buffercounter = 0;

uint8_t usart_rx_buffer[USART_BUFFER_SIZE];
uint8_t *usart_rx_buffer_pointer_in	= &usart_rx_buffer[0];
uint8_t *usart_rx_buffer_pointer_out = &usart_rx_buffer[0];

volatile uint8_t usart_tx_buffercounter = 0;
uint8_t usart_tx_buffer[USART_BUFFER_SIZE];
uint8_t *usart_tx_buffer_pointer_in = &usart_tx_buffer[0];
uint8_t *usart_tx_buffer_pointer_out = &usart_tx_buffer[0];


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
uint8_t usart_write(uint8_t b)
{
  if (usart_tx_buffercounter>=USART_BUFFER_SIZE) return 0; // Puffer ist voll

  *usart_tx_buffer_pointer_in++ = b;
  usart_tx_buffercounter++;
  UCR |= (1<<TXCIE); // Sende Interrupt ein

  if (usart_tx_buffer_pointer_in >= usart_tx_buffer+USART_BUFFER_SIZE)
  {
    // Ringpuffer wieder von vorn beginnen
    usart_tx_buffer_pointer_in = usart_tx_buffer;
  }
  return 0x80;  
}

//Routine für das schreiben eines Puffers
uint8_t usart_write_buffer(const uint8_t *buffer, uint8_t len)
{
  while (len-->0) 
  {
    uint8_t retval = usart_write(*buffer++);
    if (retval!=0x80) return retval;
  }
  return 0x80;  
}

//----------------------------------------------------------------------------
//Routine für das Lesen eines Bytes
uint8_t usart_read()
{
  uint8_t b = 0;
  if(usart_rx_buffercounter>0)
  {
    // Gelesen wird über den Ausgabepointer
    b = *usart_rx_buffer_pointer_out++;
    usart_rx_buffercounter--;
    if (usart_rx_buffer_pointer_out >= usart_rx_buffer+USART_BUFFER_SIZE)
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
  while (usart_rx_buffercounter>0 && len<maxlen)
  {
    *buffer++ = usart_read();
    len++;
  }
  return len;
}

//----------------------------------------------------------------------------
//Interrupt Service Routine zum Empfang eines Zeichens
ISR (USART_RX)
{
  unsigned char receive_char;
  receive_char = (UDR);
  
  if (usart_rx_buffercounter <= USART_BUFFER_SIZE)
  {
    // Das gelesene Zeichen kommt über den Eingabepointer in den Puffer
    *usart_rx_buffer_pointer_in++ = receive_char;
    usart_rx_buffercounter++;
    if (usart_rx_buffer_pointer_in >= usart_rx_buffer+USART_BUFFER_SIZE)
    {
      // Ringpuffer wieder von vorn beginnen
      usart_rx_buffer_pointer_in = usart_rx_buffer;
    }
  }
}

// Interrupt Service Routine zum Senden eines Zeichens
ISR(USART_TX)
{
  if (usart_tx_buffercounter)
  {
    usart_tx_buffercounter--;
    UDR = usart_tx_buffer_pointer_out++; // nächstes Zeichen aus Puffer senden
    if (usart_tx_buffer_pointer_out >= usart_tx_buffer+USART_BUFFER_SIZE)
    {
      // Ringpuffer wieder von vorn beginnen
      usart_tx_buffer_pointer_out = usart_tx_buffer;
    }
  }
  if (usart_tx_buffercounter==0) UCR &= ~(1<<TXCIE); // Sende Interrupt aus wenn leer
}



