/*----------------------------------------------------------------------------
Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
Author:         Radig Ulrich
Remarks:
known Problems: none
Version:        24.10.2007
Description:    RS232 Routinen

Dieses Programm ist freie Software. Sie können es unter den Bedingungen der
GNU General Public License, wie von der Free Software Foundation veröffentlicht,
weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder
(nach Ihrer Option) jeder späteren Version.

Die Veröffentlichung dieses Programms erfolgt in der Hoffnung,
daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE,
sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT
FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License.

Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
Programm erhalten haben.
Falls nicht, schreiben Sie an die Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
------------------------------------------------------------------------------*/

// Benutzung:
// Datei einbinden: #include "usart.h"
// Am Anfang von main() usart_init(baudrate); aufrufen
// sei(); nicht vergessen, wenn etwas empfangen werden soll.

// Die Empfangsroutine arbeitet wie ein Terminal mit Echo
// Backspace wird ausgewertet
// Bei Zeilenschaltung wird usart_status.usart_ready gesetzt.
// Dann muss vom Programm der usart_rx_buffer ausgewertet werden
// Nach dem Auswerten die Flags zurücksetzen!

#ifndef _UART_H
#define _UART_H

#include <stdio.h>

#define USART_ECHO	1

#define BUFFER_SIZE	50

volatile unsigned char buffercounter;
char usart_rx_buffer[BUFFER_SIZE];

char *rx_buffer_pointer_in;
char *rx_buffer_pointer_out;

struct {
	unsigned char usart_ready:1;
	unsigned char usart_rx_ovl:1;
	unsigned char usart_disable:1; //benötigt für ftp2com
} volatile usart_status ;

//----------------------------------------------------------------------------

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
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

void usart_init(unsigned long baudrate);
void usart_write_char(char c);
void usart_write_str(char *str);

void usart_write_P (const char *Buffer,...);
#define usart_write(format, args...)   usart_write_P(PSTR(format) , ## args)

// dies stdout zuweisen um die Ausgabe umzulenken.
// stdout = &usart_stdout;
extern FILE usart_stdout;

//----------------------------------------------------------------------------

#endif //_UART_H
