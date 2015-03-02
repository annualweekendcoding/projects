//***************************************************************************
//
//  File........: sound.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: GCC 4.1.1; avr-libc 1.4.5
//
//  Description.: AVR Butterfly sound routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//  20031009          port to gcc/avr-libc                          - M.Thomas
//  20040123          added temp.-var.                              - n.n./mt
//  20050727          added local helper-function                   - mt
//  20060107          fixed or in pause-if                          - mt
//
//***************************************************************************

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "main.h"
#include "sound.h"


/*****************************************************************************
*
*   Function name : Sound_Init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Set up Timer1 with PWM
*
*****************************************************************************/
void Sound_Init(void)
{
    TCCR1A = (1<<COM1A1);// | (1<<COM1A0); // Set OC1A when upcounting, clear when downcounting
    TCCR1B = (1<<WGM13);        // Phase/Freq-correct PWM, top value = ICR1

    sbiBF(TCCR1B, CS10);             // start Timer1, prescaler(1)

    OCR1AH = 0;    // Set a initial value in the OCR1A-register
    OCR1AL = 80;   // This will adjust the volume on the buzzer, lower value => higher volume
    
    DDRB |= (1<<PB5); // OC1A Ausgang aktivieren
}

/*****************************************************************************
*
*   Function name : Play_Tune
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Plays the song
*
*****************************************************************************/
void Play_Sound(unsigned int temp_tone)
{
    int temp_hi;

    if( (temp_tone == p) || (temp_tone == P) ) // if pause
        cbiBF(TCCR1B, CS10);             // stop Timer1, prescaler(1)
    else
        sbiBF(TCCR1B, CS10);             // start Timer1, prescaler(1)

    cli(); // mt __disable_interrupt();

    // mt temp_hi = *(pSong + Tone);      // read out the PWM-value
    // temp_hi = pgm_read_word(pSong + Tone);      // read out the PWM-value
    temp_hi = temp_tone;   // mt 200301
    temp_hi >>= 8;                  // move integer 8 bits to the rigth

    TCNT1H = 0;                     // reset TCNT1H/L
    TCNT1L = 0;

    ICR1H = temp_hi;                // load ICR1H/L
    // mt: ICR1L = *(pSong + Tone);
    // ICR1L = pgm_read_word(pSong + Tone);
    ICR1L = temp_tone;

    sei(); // mt: __enable_interrupt();
}

void Sound_Off(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
}
