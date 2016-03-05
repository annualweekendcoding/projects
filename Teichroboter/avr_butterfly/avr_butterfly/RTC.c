//*****************************************************************************
//
//  File........: RTC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.1.1; avr-libc 1.4.5
//
//  Description.: Real Time Clock (RTC)
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20051107          minior correction (volatiles)                 - mt
//  20070129          SIGNAL->ISR                                   - mt
//*****************************************************************************

//mtA
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "RTC.h"

volatile uint8_t  gSECOND;
volatile uint8_t  gMINUTE;
volatile uint8_t  gHOUR;

volatile uint8_t Timer_Second = 0;


/******************************************************************************
*
*   Function name:  RTC_init
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        Start Timer/Counter2 in asynchronous operation using a
*                   32.768kHz crystal.
*
*******************************************************************************/
void RTC_init(void)
{
    _delay_ms(1000);            // wait for 1 sec to let the Xtal stabilize after a power-on,

    cli(); // mt __disable_interrupt();  // disabel global interrupt

    TIMSK2 &= ~(1<<TOIE2);             // disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        // select asynchronous operation of Timer2

    TCNT2 = 0;              // clear TCNT2A
    TCCR2A |= (1<<CS22) | (1<<CS20);             // select precaler: 32.768 kHz / 128 = 1 sec between each overflow

    while((ASSR & 0x01) | (ASSR & 0x04));       // wait for TCN2UB and TCR2UB to be cleared

    TIFR2 = 0xFF;           // clear interrupt-flags
    TIMSK2 |= (1<<TOIE2);     // enable Timer2 overflow interrupt

    sei(); // mt __enable_interrupt();                 // enable global interrupt

    // initial time and date setting
    gSECOND  = 0;
    gMINUTE  = 0;
    gHOUR    = 0;
}


/******************************************************************************
*
*   Timer/Counter2 Overflow Interrupt Routine
*
*   Purpose: Increment the real-time clock
*            The interrupt occurs once a second (running from the 32kHz crystal)
*
*******************************************************************************/
// mtA
// #pragma vector = TIMER2_OVF_vect
// __interrupt void TIMER2_OVF_interrupt(void)
// SIGNAL(SIG_OVERFLOW2)
ISR(TIMER2_OVF_vect)
// mtE
{
    Timer_Second++;
    
    if (gSECOND > 0 || gMINUTE > 0 || gHOUR > 0)
    {
        if (gSECOND == 0)
        {
            gSECOND = 59;
            if (gMINUTE == 0)
            {
                gMINUTE = 59;
                if (gHOUR > 0)
                {
                    gHOUR--;
                }
            }
            else
            {
                gMINUTE--;
            }

        }
        else
        {
            gSECOND--;
        }
    }
}
