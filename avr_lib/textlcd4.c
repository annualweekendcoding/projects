/* How to control a 4-Line HD44780-based Character-LCD
 * with 4-bit-Bus
 *
 * Chip:                    ATMEL ATTINYS2313
 * Clock:                   4 MHz
 * Compiler:                avr-gcc
 *
 * ATMEL PIN                LCD PIN
 *
 * PB3(15)                  R/W(5)
 *
 * PD0(2)                   D4(12)
 * PD1(3)                   D5(13)
 * PD2(6)                   D6(14)
 * PD3(7)                   D7(15)
 * PD4(8)                   E1(6)
 * PD5(9)                   E2(7)
 * PD6(11)                  RS(4)
 *
 * Written by:              Jon Wackley (VE3JTN)
 * Date:                    November 3rd 2002
 * Edited by:				Mirko Wittek
 * Date:					2008/02/09
 */

#include "textlcd4.h"
#include <util/delay.h>
#include <stdlib.h>
#include "textlcd4_config.h"
#include <avr/interrupt.h>

#define LCD_RS_MASK         ( 0x01 << LCD_RS_BIT )
#define LCD_E1_MASK         ( 0x01 << LCD_E1_BIT )
#define LCD_E2_MASK         ( 0x01 << LCD_E2_BIT )
#define LCD_MASK            ( LCD_RS_MASK | LCD_E1_MASK | LCD_E2_MASK )
#define LCD_RW_MASK         ( 0x01 << LCD_RW_BIT )

typedef uint8_t BOOL;
typedef uint8_t BYTE;

// Dies ist die aktuelle Displayhaelfte
BYTE lcd_akt_e = 0;

// aktuelle Displayposition für step-Funktionen
volatile uint8_t lcd_pos;

#define lcd_enable() { LCD_PORT |= lcd_akt_e;}
#define lcd_disable() { LCD_PORT &= ~(LCD_E1_MASK | LCD_E2_MASK); }

#define lcd_upper() { lcd_akt_e = LCD_E1_MASK; }
#define lcd_lower() { lcd_akt_e = LCD_E2_MASK; }

/*
 * Send one pulse to the E signal (enable).  Mind the timing
 * constraints.  If readback is set to true, read the HD44780 data
 * pins right before the falling edge of E, and return that value.
 */
uint8_t lcd_pulse_e(bool readback)
{
  uint8_t x;

  lcd_enable();
  /*
   * Guarantee at least 500 ns of pulse width.  For high CPU
   * frequencies, a delay loop is used.  For lower frequencies, NOPs
   * are used, and at or below 1 MHz, the native pulse width will
   * already be 1 us or more so no additional delays are needed.
   */
#if F_CPU > 4000000UL
  _delay_us(0.5);
#else
  /*
   * When reading back, we need one additional NOP, as the value read
   * back from the input pin is sampled close to the beginning of a
   * CPU clock cycle, while the previous edge on the output pin is
   * generated towards the end of a CPU clock cycle.
   */
  if (readback)
    __asm__ volatile("nop");
#  if F_CPU > 1000000UL
  __asm__ volatile("nop");
#    if F_CPU > 2000000UL
  __asm__ volatile("nop");
  __asm__ volatile("nop");
#    endif /* F_CPU > 2000000UL */
#  endif /* F_CPU > 1000000UL */
#endif
  if (readback)
    x = LCD_PIN & 0xF;
  else
    x = 0;
  lcd_disable();

  return x;
}

static inline void lcd_configure_read () 
{
  LCD_DDR &= 0xF0;
  LCD_RW_PORT |= LCD_RW_MASK;  
}

static inline void lcd_configure_write () 
{
  LCD_RW_PORT &= ~LCD_RW_MASK;  
  LCD_DDR |= 0x0F;
}

uint8_t lcd_get ( bool command ) 
{
  lcd_configure_read();
  if ( command ) 
	{
      LCD_PORT &= ~LCD_RS_MASK;
    } 
	else 
	{
      LCD_PORT |= LCD_RS_MASK;
  }
  uint8_t x = lcd_pulse_e(true) << 4;
  x |= lcd_pulse_e(true);
  lcd_configure_write();
  return x;
}

void lcd_busy() { while(lcd_get(true) & 0x80 ); }


/* lcd_put
 *
 * Purpose:     - Writes characters or commands to LCD.
 *              - Used with lcd_put_command and lcd_put_char macros
 *              - BOOL command indicates whether char or command is output
 *              - value is output to LCD
 *
 */


void lcd_put ( uint8_t value, bool command ) 
{
  lcd_busy();
  if ( command ) 
	{
      LCD_PORT &= ~LCD_RS_MASK;
  } 
	else 
	{
    LCD_PORT |= LCD_RS_MASK;
  }
  LCD_PORT = (LCD_PORT & 0xF0) | (value >> 4);
  lcd_pulse_e(false);
  LCD_PORT = (LCD_PORT & 0xF0) | (value & 0x0F);
  lcd_pulse_e(false);
}



/*
 * Purpose:     - Sets the Character-Generator-RAM address.
 *                      CGRAM data is read/written after this setting.
 *              - Required CGRAM address must be set
 *
 * b0-5 : required CGRAM address
 * b6-7 : don't care
 *
 */


void lcd_init()
{
  // Die Initialisierung bezieht sich gleich auf beide Displayhälften
  lcd_akt_e = LCD_E1_MASK | LCD_E2_MASK;

  // RW-Bit auf Ausgang schalten
  LCD_RW_DDR |= LCD_RW_MASK;
  lcd_configure_write();
  
  // 3 Steuerbits auf Ausgang schalten
  LCD_DDR = LCD_MASK;

  // 3 x 3 schreiben entsprechend Benutzerhandbuch
  LCD_PORT = (LCD_PORT & 0xF0) | 3;
  lcd_pulse_e(false);
  _delay_ms(5);
  lcd_pulse_e(false);
  _delay_ms(1);
  lcd_pulse_e(false);
  _delay_us(LCD_WRITE_DELAY_US);
  lcd_busy();
  // 2 schreiben, wegen 4-Bit-Modus
  LCD_PORT = (LCD_PORT & 0xF0) | 2;
  lcd_pulse_e(false);
  // 8 schreiben, für 2-Zeiliges-Display, 4 für 5x10 Zeichenbox
  LCD_PORT = (LCD_PORT & 0xF0) | 8 | 4;
  lcd_pulse_e(false);

  lcd_lower(); 
  lcd_clear();
  lcd_put_command(LCD_CMDS_DISPLAY_MODE_ON);
  lcd_put_command(LCD_CMDS_ENTRY_MODE_INC_CURSOR);
  lcd_set_display_address(0);
  lcd_upper();
  lcd_clear();
  lcd_put_command(LCD_CMDS_DISPLAY_MODE_ON);
  lcd_put_command(LCD_CMDS_ENTRY_MODE_INC_CURSOR);
  lcd_set_display_address(0);
  lcd_pos = 0;
}

void lcd_print(const char *text)
{
  while (*text) lcd_put_char(*text++);
}

void lcd_set_cursor(uint8_t pos, uint8_t line)
{
  if (line>1) 
  {
    lcd_lower();
  }
  else 
  {
    lcd_upper();
  }
  lcd_set_display_address( ((line&1) ? 64 :0) + pos);  
}



void lcd_printInt(int z)
{
  char buf[7];
  itoa(z,buf,10);
  lcd_print(buf);  
}



void lcd_printHex(uint8_t z)
{
  char buf[3];
  itoa(z,buf,16);
  // Wenn nur eine Stelle, dann 0 davor setzen
  if (!buf[1]) lcd_put_char('0');
  lcd_print(buf);  
}

void lcd_printFixed(int z, uint8_t digits, uint8_t commadigits)
{
  // z - Auszugebende Zahl
  // digits - Anzahl der Stellen vor dem Komma
  // commadigits - Anzahl der Stellen nach dem Komma
  char buf[7];
  itoa(z,buf,10);
  uint8_t l = 0;
  char *s=buf;
  while (*s++) l++;
  if (l>commadigits) l-=commadigits;
  for (uint8_t i=l; i<digits; i++)
  {
    lcd_put_char(' ');
  }
  uint8_t i=0;
  for (; i<l; i++)
  {
    lcd_put_char(buf[i]);
  }
  if (commadigits>0) 
  {
    lcd_put_char('.');
    for (i=0; i<commadigits; i++)
    {
      char c = buf[i+l];
      if (!c) break;
      lcd_put_char(c);
    }
    for (;i<commadigits; i++)
    {
      lcd_put_char('0');
    }
  }
}

void lcd_step_cursor(uint8_t step)
{
  lcd_pos+=step;
  if (lcd_pos>=(LCD_LINECOUNT*LCD_ROWCOUNT)) lcd_pos=0;
}

void lcd_step_char(uint8_t c)
{
  lcd_set_cursor(lcd_pos%LCD_ROWCOUNT, lcd_pos/LCD_ROWCOUNT);
  lcd_put_char(c);
  lcd_step_cursor(1);
}

void lcd_step_print(const char *text)
{
  while (*text) lcd_step_char(*text++);
}

void lcd_step_printInt(int z)
{
  char buf[7];
  itoa(z,buf,10);
  lcd_step_print(buf);  
}

void lcd_step_printHex(uint8_t z)
{
  char buf[3];
  itoa(z,buf,16);
  // Wenn nur eine Stelle, dann 0 davor setzen
  if (!buf[1]) lcd_step_char('0');
  lcd_step_print(buf);  
}

void newline()
{
  lcd_step_cursor(LCD_ROWCOUNT);
  lcd_pos *= (lcd_pos/LCD_ROWCOUNT); // abrunden auf Zeilenanfang
  lcd_set_cursor(lcd_pos%LCD_ROWCOUNT, lcd_pos/LCD_ROWCOUNT);
}

