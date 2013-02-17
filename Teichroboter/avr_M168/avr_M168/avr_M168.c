/*
 * avr_M168.c
 *
 * Created: 16.02.2013 14:07:22
 *  Author: mirko
 */ 


#include <avr/io.h>
#include "apod_layer2.h"
#include "led_config.h"
#include "led.h"
//#include "timer.h"

int main(void)
{
	led_init();
	usart_init(9600);
	apod_layer2_init();
	timer_init();

	//Globale Interrupts einschalten
	sei();
	
	uint8_t buffer[10];
	
    while(1)
    {
		if (usart_read_len()>=10)
		{
			usart_read_buffer(buffer,10);
  		    LED_FLASH(RED);
			if (buffer[0]) LED_ON(GREEN);
			else LED_OFF(GREEN);  
			if (buffer[1])
			{
				usart_write_buffer("Hallo 1",7);
			}
		}
    }
}