#ifndef _USART_STR_H
#define _USART_STR_H

#include <stdio.h>
#include <avr/io.h>

#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
//----------------------------------------------------------------------------

void usart_write_char(char c);
void usart_write_str(char *str);

void usart_printf_P (const char *Buffer,...);
#define usart_printf(format, args...)   usart_printf_P(PSTR(format) , ## args)

// dies stdout zuweisen um die Ausgabe umzulenken.
// stdout = &usart_stdout;
extern FILE usart_stdout;

//----------------------------------------------------------------------------
#endif //_USART_STR_H
