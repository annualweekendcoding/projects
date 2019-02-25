/*
* avr_M168.c
*
* Created: 16.02.2013 14:07:22
*  Author: mirko
*/


#include <avr/io.h>
#include "usart_raw.h"
#include "usart_str.h"
#include "led_config.h"
#include "led.h"
#include "timer.h"
#include "modbus.h"

#define EN_DDR DDRB
#define EN_PORT PORTB
#define EN_BIT PB3

int main(void)
{
  led_init();
  //usart_init(9600);
  timer_init();
  
  // Ausgang für Enable freischalten
  ( EN_DDR |= (1<<EN_BIT) );

  //Globale Interrupts einschalten
  sei();
  short int state=0;
  int timer1=time_ms;
  int pause_ms = 1;
  int drehzeit = 1000;
  while (1) 
  {
    short int hall = PINB & (1<<PB2);
    
    switch (state)
    {
      case 0:
        if (hall) state=1; else state=2;
        break;
      case 1:
        if (!hall) state=3;
        break;
      case 2:     
        if (hall) state=4;
        break;
      case 3:
        timer1=time_ms;
        state=5;
        break;
      case 4:     
        timer1=time_ms;
        state=6;
        break;
      case 5:
        if ((time_ms-timer1)>pause_ms) state=2;
        break;
      case 6:     
        if ((time_ms-timer1)>pause_ms) state=1;
        break;
    }    
    if (state==1 || state==3 || state ==5) 
    { LED_ON(RED); LED_OFF(GREEN); } else { LED_OFF(RED); LED_ON(GREEN); }
    if (state==1 || state==2) (EN_PORT |= (1<<EN_BIT)); else ( EN_PORT &= ~(1<<EN_BIT) );
  }
    
  /*
  uint8_t modbus_crc_errors_saved=modbus_crc_errors;
  LED_FLASH(GREEN);
  while (1) 
  {
    modbus_words[1] = time_ms;
    modbus_cycle();
    if (modbus_words[0]) LED_ON(GREEN); else LED_OFF(GREEN);
    if (modbus_crc_errors!=modbus_crc_errors_saved) LED_FLASH(RED);
    modbus_crc_errors_saved=modbus_crc_errors;
  }
  */
      
/*
  usart_write_str("AVR started\r\n");
  
  while(1)
  {
    uint16_t recdelay = usart_rx_getdelay();
    if (usart_read_len()>0 && recdelay>1) 
    {
      LED_FLASH(RED);
      //uint8_t buffer[10];
      while (usart_read_len()>0) usart_write(usart_read());
      usart_printf("\r\n");
      _delay_ms(100);
    }
    else if (recdelay>1000)
    {
      usart_printf("Delay %i\r\n",recdelay);
      _delay_ms(1000);
    }    
  }
*/  
}