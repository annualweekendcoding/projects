#ifndef _TIMER_H
#define _TIMER_H

#include <avr/io.h>
extern volatile int time_sec;
extern volatile int time_ms;
void timer_init (void);
 
#endif //_TIMER_H

