//*****************************************************************************
//
//  File........: RTC.h
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Description.: Functions for RTC.c
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - File created                                  - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//
//*****************************************************************************


extern volatile uint8_t  gSECOND;
extern volatile uint8_t  gMINUTE;
extern volatile uint8_t  gHOUR;

extern volatile uint8_t  gPowerSaveTimer;
extern volatile uint8_t  gStartDelay;

//  Function declarations
void RTC_init(void);            //initialize the Timer Counter 2 in asynchron operation
void Time_update(void);        //updates the time and date
