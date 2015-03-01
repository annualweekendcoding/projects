// Benutzung:
// usart_raw_config.template in das Projekt als usart_raw_config.h kopieren
// Datei einbinden: #include "usart_raw.h"
// Am Anfang von main() usart_init(baudrate); aufrufen
// sei(); nicht vergessen, wenn etwas empfangen werden soll.

#ifndef _USART_RAW_H
#define _USART_RAW_H

#include <stdio.h>
#include "usart_raw_config.h"

volatile uint8_t usart_rx_buffercounter;
volatile uint8_t usart_tx_buffercounter;

//----------------------------------------------------------------------------

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <avr/io.h>

//----------------------------------------------------------------------------

//Anpassen der seriellen Schnittstellen Register wenn ein ATMega128 benutzt wird
//TODO: Die Definitionen prüfen, sie scheinen nicht vollständig zu sein
#if defined (__AVR_ATmega128__)
#define USR UCSR0A
#define UCR UCSR0B
#define UDR UDR0
#define UBRR UBRR0L
#define USART_RX USART0_RX_vect
#define USART_TX USART0_TX_vect
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
#define USR UCSR0A
#define UCR UCSR0B
#define UBRR UBRR0L
#define EICR EICRB
#define TXEN TXEN0
#define RXEN RXEN0
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define TXC TXC0
#define RXC RXC0
#define UDR UDR0
#define UDRE UDRE0
#define USART_RX USART0_RX_vect
#define USART_TX USART0_TX_vect
#elif defined (__AVR_ATmega32__)
#define USR UCSRA
#define UCR UCSRB
#define UBRR UBRRL
#define EICR EICRB
#define USART_RX USART_RXC_vect
#define USART_TX USART_TXC_vect
#elif defined (__AVR_ATmega8__)
#define USR UCSRA
#define UCR UCSRB
#define UBRR UBRRL
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
#define USR UCSR0A
#define UCR UCSR0B
#define UBRR UBRR0L
#define TXEN TXEN0
#define RXEN RXEN0
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define TXC TXC0
#define RXC RXC0
#define UDR UDR0
#define UDRE UDRE0
#define USART_RX USART_RX_vect
#define USART_TX USART_TX_vect
#else
#error USART-Library nicht fuer diesen Prozessor parametriert
#endif
//----------------------------------------------------------------------------

// Schnittstelle initialisieren
void usart_init(unsigned long baudrate);

// Schreibt ein einzelnes Zeichen  
// liefert 0x80 bei Erfolg
uint8_t usart_write(uint8_t b);

// Schreibt eine Reihe von Zeichen
// liefert 0 bei Erfolg und die Anzahl von Restzeichen die nicht gesendet werden konnten
uint8_t usart_write_buffer(const uint8_t* buffer, uint8_t len);

// Schreibt eine Reihe von Zeichen aus dem Programmspeicher
// liefert 0 bei Erfolg und die Anzahl von Restzeichen die nicht gesendet werden konnten
uint8_t usart_write_buffer_p(const uint8_t *buffer, uint8_t len);

#if USART_RX_BUFFER_SIZE>0
  // liefert die Restlänge zu sendender Zeichen
  #define usart_write_len() (usart_tx_buffercounter)
#else
  // ohne Puffer wird 1 geliefert, wenn das letzte Zeichen noch nicht gesendet ist, sonst 0
  #define usart_write_len() ((USR & (1<<UDRE))?1:0)
#endif

// Liest ein einzelnes Zeichen - liefert 0 wenn Puffer leer
uint8_t usart_read();

// Leert den Empfangspuffer
void usart_flush();

// Liest in einen Puffer - liefert die gelesende Länge
uint8_t usart_read_buffer(uint8_t* buffer, uint8_t maxlen);

#if USART_RX_BUFFER_SIZE>0
  // mit Puffer wird die Anzahl der Zeichen im Puffer zurückgeliefert
  #define usart_read_len() (usart_rx_buffercounter)
#else
  // ohne Puffer wird 1 zurückgeliefert, wenn ein Zeichen im Register ist, sonst 0
  #define usart_read_len() ((USR & (1 << RXC))?1:0)
#endif

#ifdef USART_SAVE_LASTTIME
  extern volatile uint16_t usart_rx_lasttime;
  // liefert die Zeitverzögerung seit dem letzten Empfang
  #define usart_rx_getdelay() (time_ms-usart_rx_lasttime)
#endif

//----------------------------------------------------------------------------
#endif //_USART_RAW_H
