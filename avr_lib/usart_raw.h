// Benutzung:
// Datei einbinden: #include "usart_raw.h"
// Am Anfang von main() usart_init(baudrate); aufrufen
// sei(); nicht vergessen, wenn etwas empfangen werden soll.

#ifndef _USART_RAW_H
#define _USART_RAW_H

#include <stdio.h>

#define BUFFER_SIZE	50
volatile uint8_t usart_buffercounter;

//----------------------------------------------------------------------------

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <avr/io.h>

//----------------------------------------------------------------------------

//Anpassen der seriellen Schnittstellen Register wenn ein ATMega128 benutzt wird
#if defined (__AVR_ATmega128__)
#define USR UCSR0A
#define UCR UCSR0B
#define UDR UDR0
#define UBRR UBRR0L
#define USART_RX USART0_RX_vect
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
#define USR UCSR0A
#define UCR UCSR0B
#define UBRR UBRR0L
#define EICR EICRB
#define TXEN TXEN0
#define RXEN RXEN0
#define RXCIE RXCIE0
#define UDR UDR0
#define UDRE UDRE0
#define USART_RX USART0_RX_vect
#elif defined (__AVR_ATmega32__)
#define USR UCSRA
#define UCR UCSRB
#define UBRR UBRRL
#define EICR EICRB
#define USART_RX USART_RXC_vect
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
#define UDR UDR0
#define UDRE UDRE0
#define USART_RX USART_RX_vect
#else
#error USART-Library nicht fuer diesen Prozessor parametriert
#endif
//----------------------------------------------------------------------------

// Schnittstelle initialisieren
void usart_init(unsigned long baudrate);

// Schreibt ein einzelnes Zeichen 
void usart_write(uint8_t b);

// Schreibt eine Reihe von Zeichen
void usart_write_buffer(const uint8_t* buffer, uint8_t len);

// Liest ein einzelnes Zeichen - liefert 0 wenn Puffer leer
uint8_t usart_read();

// Liest in einen Puffer - liefert die gelesende Länge
uint8_t usart_read_buffer(uint8_t* buffer, uint8_t maxlen);

// Die aktuelle zu lesende Länge steht im buffercounter
#define usart_read_len() (usart_buffercounter)

//----------------------------------------------------------------------------

#endif //_USART_RAW_H
