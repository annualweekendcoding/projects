#include "timer.h"
#include <avr/interrupt.h>

volatile int time_ms;
volatile int time_sec;

//----------------------------------------------------------------------------
//Diese Routine startet und initialisiert den Timer
void timer_init (void)
{
  // Clear on Compare-Mode
	TCCR1B |= (1<<WGM12);
  // Prescaler auf 1
  TCCR1B |= (0<<CS12 | 0<<CS11 | 1<<CS10);
  // externer Takt 
	//TCCR1B |= (1<<CS12 | 1<<CS11 | 1<<CS10);
	TCNT1 = 0;
	OCR1A = 7999;
	//OCR1A = 2499;
	TIMSK1 |= (1 << OCIE1A);
};

//----------------------------------------------------------------------------
//Timer Interrupt
ISR (TIMER1_COMPA_vect)
{
  static int time_ms_1000;
  //Millisekunde um 1 erhöhen
  time_ms++;
  time_ms_1000++;
  if (time_ms_1000>=1000)
  {
    time_ms_1000=0;
    //Sekunde um 1 erhöhen
    time_sec++;
  }
}

