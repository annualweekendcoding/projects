/*
* avr_M168.c
*
* Created: 16.02.2013 14:07:22
*  Author: mirko
*/


#include <avr/io.h>
#include "usart_raw.h"
#include "apod_layer2.h"
#include "led_config.h"
#include "led.h"
#include "timer.h"

int main(void)
{
  led_init();
  usart_init(9600);
  apod_layer2_init();
  timer_init();

  //Globale Interrupts einschalten
  sei();
  
  while(1)
  {
    apod_layer2_tick();
    if (usart_read_len()>0) LED_FLASH(RED);
    if (apod_receive_state==receive_data)
    {
      if (apod_receive_buffer.len==1)
      {
        if (apod_receive_buffer.data[0]==0) LED_OFF(GREEN);
        if (apod_receive_buffer.data[0]==1) LED_ON(GREEN);
        if (apod_receive_buffer.data[0]==2) LED_TOGGLE(GREEN);
        if (apod_receive_buffer.data[0]==3) LED_FLASH(GREEN);
      }
      apod_receive_clear();
    }
  }
}