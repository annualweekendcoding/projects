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

int main(void)
{
  led_init();
  usart_init(9600);
  timer_init();

  //Globale Interrupts einschalten
  sei();
  
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
}