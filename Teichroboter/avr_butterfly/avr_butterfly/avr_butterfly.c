//***************************************************************************
//
//
//***************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/version.h>
#include <util/delay.h>

#include "main.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "button.h"
#include "BCD.h"
#include "sound.h"
#include "ADC.h"
#include "USI_TWI_master.h"
#include "usart.h"
#include "globalvar.h"
#include "teranis.h"

#define pLCDREG_test (*(char *)(0xEC))

uint8_t PowerSave = 0;
uint8_t SleepIdle = 0;
uint8_t akku_niedrig = 0;
int8_t rechts_pwm = 0;
int8_t links_pwm = 0;
uint8_t pumpe_ein = 0;
int16_t kompass_grad = -1;
uint16_t akku_spannung = 0;
uint8_t anlage_aus = 1;
uint8_t pumpe_ausgeklappt = 1;
int8_t temperatur = 0;
int16_t kurs_plan = 0;
int16_t kurs_akt = 0;
int16_t kurs_diff = 0;
int8_t geschwindigkeit = 0;
uint8_t statustextnr = 0;
uint8_t antriebsfreigabe = 0;
uint8_t init = 0;
int16_t drehversuche = 0;

uint16_t kompass_roh = -1;
uint16_t akku_spannung_roh = 0;

const char statustext0[] PROGMEM = "UNDEF";
const char statustext1[] PROGMEM = "AUS";
const char statustext2[] PROGMEM = "DRHINT";
const char statustext3[] PROGMEM = "FAHINT";
const char statustext4[] PROGMEM = "DRVORN";
const char statustext5[] PROGMEM = "FAVORN";
const char statustext6[] PROGMEM = "DRSTOE";

const char* const statustext[] PROGMEM = { statustext0, statustext1, statustext2, statustext3, statustext4, statustext5, statustext6 };

void Beep(void);

/*****************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Contains the main loop of the program
*
*****************************************************************************/
// mt __C_task void main(void)
int main(void)
{
  char input;
  uint8_t mode=0;

  uint8_t doBeep = 0;    // Piepton beim Erreichen von 0

  // Program initalization
  Initialization();
  sei();

  //usart_write_P(PSTR("Teichroboter start\r\n"));
  for (;;)            // Main loop
  {
    if(!PowerSave)          // Do not enter main loop in power-save
    {
      input = getkey();           // Read buttons

      cli();

      if (input != KEY_NULL)
      {
        if (input == KEY_NEXT)
        {
          //PlayClick();
          if (mode==2) rechts_pwm += 10;
          if (mode==3) links_pwm += 10;
          if (mode==4) pumpe_ein = 1;
        }
        else if (input == KEY_PREV)
        {
          //PlayClick();
          if (mode==2) rechts_pwm -= 10;
          if (mode==3) links_pwm -= 10;
          if (mode==4) pumpe_ein = 0;
        }
        else if (input == KEY_PLUS)
        {
          //PlayClick();
          mode++;
          if (mode>7) mode=0;
        }
      }

      sei();
      
      // Der Analogeingang ist so kalibriert, dass bei 20,48V das Maximum 1024 erreicht wird.
      // Die Akku-Spannung wird hier also in 0,01V angezeigt.
      akku_spannung_roh = ADC_Read(1) << 1;
      // gleitenden Mittelwert bilden
      akku_spannung = (akku_spannung*9 + akku_spannung_roh)/10;

      // internen Temperatursensor auslesen, Umrechnung in °C entsprechend Manual
      //int16_t temperatur_adc = ADC_Read(0); 
      //temperatur = 3450.0 / (log(temperatur_adc/(1024.0-temperatur_adc)+3450.0/298.0)) - 273.0;
      
      // Kompass auslesen
      unsigned char msg[3];
      msg[0] = 0x42; 
      msg[1] = 'A';
      if (USI_TWI_Start_Transceiver_With_Data(msg,2))
      {
        _delay_ms(7); // Dem Kompass >6ms Zeit lassen um das Ergebnis zu ermitteln
        msg[0] = 0x42 | (1<<TWI_READ_BIT);
        msg[1] = 0;
        msg[2] = 0;
        if (USI_TWI_Start_Transceiver_With_Data(msg,3))
        {
          // Der Wert liegt in 2 Bytes vor.
          kompass_roh = msg[1]<<8 | msg[2];
          // gleitenden Mittelwert bilden
          kompass_grad = (kompass_grad*9 + kompass_roh)/10;  
        }
        else kompass_grad = -1;
      }      
      else kompass_grad = -1;
            
      // Eingänge einlesen
      anlage_aus = (PINE & (1<<PE6))!=0;
      pumpe_ausgeklappt = (PINF & (1<<PF7)) !=0;

      teranis_zyklus();

      if (anlage_aus)
      {
        // Motoren aus 
        OCR0A = 255;
        OCR2A = 255;
      }
      else
      {        
        // motor_rechts an PWM ausgeben, bei negativer Drehzahl Rückwärts-Ausgang setzen
        if (rechts_pwm>=0)
        {
          OCR0A = 255-(2*rechts_pwm);
          PORTF &= ~(1<<PF4);
        }
        else
        {
          OCR0A = 255-(2*-rechts_pwm);
          PORTF |= 1<<PF4;
        }
      
        // motor_rechts an PWM ausgeben, bei negativer Drehzahl Rückwärts-Ausgang setzen
        if (links_pwm>=0)
        {
          OCR2A = 255-(2*links_pwm);
          PORTF &= ~(1<<PF5);
        }
        else
        {
          OCR2A = 255-(2*-links_pwm);
          PORTF |= 1<<PF5;
        }
      }      
        
      // Pumpe EIN ausgeben
      if (pumpe_ein) PORTF |= 1<<PF6;
      else PORTF &= ~(1<<PF6);
            
      if (doBeep)
      {
        Beep();
        // Nicht nochmal piepen
        doBeep = 0;
      }
      
      // Statustextauswahl
      if (statustextnr>=sizeof(statustext))
      {
        statustextnr=0; 
      }
      const char* statustext_akt = (const char*)( pgm_read_word( &(statustext[statustextnr]) ) );
      
      // Ausgabe auf LCD entsprechend mode
      char buffer[20];
      if (mode==0) sprintf_P(buffer,statustext_akt);
      else if (mode==7) sprintf_P(buffer,PSTR("%ds"),time_sec);
      else if (mode==6) sprintf_P(buffer,PSTR("%d %d %d"),mode,pumpe_ausgeklappt,anlage_aus);
      else if (mode==5) sprintf_P(buffer,PSTR("%d %dG"),mode,kompass_grad);
      else if (mode==4) sprintf_P(buffer,PSTR("%d %dE"),mode,pumpe_ein);
      else if (mode==3) sprintf_P(buffer,PSTR("%d %dP"),mode,links_pwm);
      else if (mode==2) sprintf_P(buffer,PSTR("%d %dP"),mode,rechts_pwm);
      else if (mode==1) sprintf_P(buffer,PSTR("%d %d.%dV"),mode,akku_spannung/100,(akku_spannung%100)/10);
      LCD_puts(buffer,1);
      LCD_UpdateRequired(1, 0);
    }

    // go to SLEEP
    if(PowerSave)
    {

      cbiBF(LCDCRA, 7);             // disable LCD

      // mtA
      // "Manual" Power-Save sleep - replaced by avr-libc functions
      // SMCR = (3<<SM0) | (1<<SE);   // Enable Power-save mode
      // asm volatile ("sleep"::);    // __sleep();  // Go to sleep
      // mt 20070131 - avr-libc 1.4.x supports ATmega169 sleep
      set_sleep_mode(SLEEP_MODE_PWR_SAVE);
      sleep_mode();
      // mtE
      if(!(PINB & 0x40))              // press UP to wake from SLEEP
      {
        PowerSave = 0;

        for(uint8_t i = 0; i < 20; i++ ) // set all LCD segment register to the variable ucSegments
        {
          *(&pLCDREG_test + i) = 0x00;
        }

        sbiBF(LCDCRA, 7);           // enable LCD
        input = getkey();           // Read buttons
      }
    }
    else if (SleepIdle)
    {
      // mtA
      // "Manual" Power-Save sleep - replaced by avr-libc functions
      // SMCR = (1<<SE);            // Enable idle mode
      // asm volatile ("sleep"::);  // __sleep();  // Go to sleep
      // mt 20070131 - avr-libc 1.4.x supports ATmega169 sleep
      set_sleep_mode(SLEEP_MODE_IDLE);
      sleep_mode();
      // mtE
    }

    // mtA
    // SMCR-reset is not needed since (from avr-libc Manual):
    // This [the sleep_mode] macro automatically takes care to
    // enable the sleep mode in the CPU before going to sleep,
    // and disable it again afterwards
    // SMCR = 0;                       // Just woke, disable sleep

    wdt_reset();

  } //End Main loop

  return 0; // mt
}



/*****************************************************************************
*
*   Function name : Initialization
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializate the different modules
*
*****************************************************************************/
void Initialization(void)
{
  //OSCCAL_calibration();       // calibrate the OSCCAL byte

  CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable

  // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
  // CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

  // set prescaler = 4, Inter RC 8Mhz / 4 = 2Mhz
  CLKPR = (1<<CLKPS1);

  // Disable Analog Comparator (power save)
  ACSR = (1<<ACD);

  // Disable Digital input on PF0-2 (power save)
  DIDR0 = (1<<ADC0D) | (1<<ADC1D) | (1<<ADC2D);
  
  // Enable Analog input ADC0, ADC1
  ADC_Init();

  // Enable PWM für Motor rechts auf PB4 = OC0
  TCCR0A = (1<<COM0A0) | (1<<COM0A1) // set on compare match
  | (1<<WGM00) | (1<<WGM01) // fast PWM
  | (1<<CS00); // clock source clk/1
  OCR0A = 255;        // Set a initial value
  DDRB |= 1<<PB4;   // enable output on OC0 Port

  // Enable PWM für Motor links auf PB7 = OC2
  TCCR2A = (1<<COM2A0) | (1<<COM2A1) // set on compare match
  | (1<<WGM20) | (1<<WGM21) // fast PWM
  | (1<<CS20); // clock source clk/1
  OCR2A = 255;        // Set a initial value
  DDRB |= 1<<PB7;   // enable output on OC2 Port

  // Interrupt an Timer2 erlauben, da außerdem für Zeitoperationen benutzt
  TIFR2 = 0xFF;           // clear interrupt-flags
  TIMSK2 |= (1<<TOIE2);     // enable Timer2 overflow interrupt

  // Ausgänge an PORTF aktivieren
  DDRF |= 
    (1<<PF4) // rechts rückwärts
    | (1<<PF5) // links rückwärts
    | (1<<PF6); // Pumpe EIN
    
  // Pullups einschalten  
  PORTE |= (1<<PE6); // Anlage aus
  PORTF |= (1<<PF7); // Pumpe ausgeklappt

  sbiBF(DDRB, 5);               // set OC1A as output
  sbiBF(PORTB, 5);              // set OC1A high

  Button_Init();              // Initialize pin change interrupt on joystick

  LCD_Init();                 // initialize the LCD

  USI_TWI_Master_Initialise();
  
  usart_init(9600);
  
  Sound_Init();
  
  // Enable Watchdog, Reset nach 2s
  wdt_enable(WDTO_2S);
}


/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
  unsigned char calibrate = 0;
  int temp;
  unsigned char tempL;

  CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
  // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
  CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

  TIMSK2 = 0;             //disable OCIE2A and TOIE2

  ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)

  OCR2A = 200;            // set timer2 compare value

  TIMSK0 = 0;             // delete any interrupt sources

  TCCR1B = (1<<CS10);     // start timer1 with no prescaling
  TCCR2A = (1<<CS20);     // start timer2 with no prescaling

  while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

  _delay_ms(1000);    // wait for external crystal to stabilise

  while(!calibrate)
  {
    cli(); // mt __disable_interrupt();  // disable global interrupt

    TIFR1 = 0xFF;   // delete TIFR1 flags
    TIFR2 = 0xFF;   // delete TIFR2 flags

    TCNT1H = 0;     // clear timer1 counter
    TCNT1L = 0;
    TCNT2 = 0;      // clear timer2 counter

    // shc/mt while ( !(TIFR2 && (1<<OCF2A)) );   // wait for timer2 compareflag
    while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag

    TCCR1B = 0; // stop timer1

    sei(); // __enable_interrupt();  // enable global interrupt

    // shc/mt if ( (TIFR1 && (1<<TOV1)) )
    if ( (TIFR1 & (1<<TOV1)) )
    {
      temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
    }
    else
    {   // read out the timer1 counter value
      tempL = TCNT1L;
      temp = TCNT1H;
      temp = (temp << 8);
      temp += tempL;
    }

    if (temp > 6250)
    {
      OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
    }
    else if (temp < 6120)
    {
      OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
    }
    else
    calibrate = 1;   // the interRC is correct

    TCCR1B = (1<<CS10); // start timer1
  }
}



void Beep(void)
{
  LCD_puts_f(PSTR("BEEP!"), 0);

  LCD_Colon(0);

  LCD_UpdateRequired(1, 0);

  Sound_Init();

  for (int i = 0; i < 10; i++)
  {
    Play_Sound(C2);
    _delay_ms(1000);
    Play_Sound(P);
    _delay_ms(100);
  }

  Sound_Off();
}

volatile int32_t time_ms=0;
volatile int16_t time_sec=0;

ISR(TIMER2_OVF_vect)
{
  // Timer2 läuft ohne Prescaling 8bit durch
  static uint16_t counter_ms = 0;
  static uint16_t counter_s = 0;
  counter_ms++;
  if (counter_ms>=(F_CPU/256/1000))
  {
    counter_ms=0;
    time_ms++;
    if ((time_ms%100) == 0) teranis_takt_100ms();
  }
  counter_s++;
  if (counter_s>=(F_CPU/256))
  {
    counter_s=0;
    time_sec++;
  }
}