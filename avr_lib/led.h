#ifndef _LED_H
#define _LED_H

#include "led_config.h"



#define LED_INIT(LED) ( LED##_DDR |= (1<<LED##_BIT) )

#define LED_TOGGLE(LED) ( LED##_PORT ^= (1<<LED##_BIT) )

#define LED_ON(LED) ( LED##_PORT |= (1<<LED##_BIT) )

#define LED_OFF(LED) ( LED##_PORT &= ~(1<<LED##_BIT) )

#ifndef LED_FLASHTIME
#define LED_FLASHTIME 10
#endif
#ifndef LED_FLASHTIME_ON
#define LED_FLASHTIME_ON LED_FLASHTIME
#endif
#ifndef LED_FLASHTIME_OFF
#define LED_FLASHTIME_OFF 9*LED_FLASHTIME 
#endif

#define LED_FLASH(LED) { LED_ON(LED); _delay_ms(LED_FLASHTIME_ON); LED_OFF(LED); _delay_ms(LED_FLASHTIME_OFF); } 

static inline void led_init()
{
  #ifdef LED_DDR
    LED_INIT(LED);
  #endif
  #ifdef LED1_DDR
    LED_INIT(LED1);
  #endif
  #ifdef LED2_DDR
    LED_INIT(LED2);
  #endif
  #ifdef LED3_DDR
    LED_INIT(LED3);
  #endif
  #ifdef LED4_DDR
    LED_INIT(LED4);
  #endif
  #ifdef LED5_DDR
    LED_INIT(LED5);
  #endif
  #ifdef LED6_DDR
    LED_INIT(LED6);
  #endif
  #ifdef LED7_DDR
    LED_INIT(LED7);
  #endif
  #ifdef LED8_DDR
    LED_INIT(LED8);
  #endif
  #ifdef LED9_DDR
    LED_INIT(LED9);
  #endif
  #ifdef RED_DDR
    LED_INIT(RED);
  #endif
  #ifdef YELLOW_DDR
    LED_INIT(YELLOW);
  #endif
  #ifdef GREEN_DDR
    LED_INIT(GREEN);
  #endif
}

#endif
