#ifndef TEXTLCD4_H
#define TEXTLCD4_H

#include <stdbool.h>
#include <avr/io.h>

enum LCD_CMDS 
{
    LCD_CMDS_NONE = 0, 
    LCD_CMDS_CLEAR = 1, // Anzeige löschen
    LCD_CMDS_HOME = 2, // Cursor auf 0

    LCD_CMDS_ENTRY_MODE_DEC_CURSOR = 4, // Cursor beim Schreiben nach links verschieben
    LCD_CMDS_ENTRY_MODE_DEC_DISPLAY = 5, // Anzeige beim Schreiben nach links verschieben
    LCD_CMDS_ENTRY_MODE_INC_CURSOR = 6, // Cursor beim Schreiben nach rechts verschieben
    LCD_CMDS_ENTRY_MODE_INC_DISPLAY = 7, // Anzeige beim Schreiben nach rechts verschieben

    LCD_CMDS_DISPLAY_MODE_OFF = 8,
    LCD_CMDS_DISPLAY_MODE_ON = 12,
    LCD_CMDS_DISPLAY_MODE_ON_UL = 14,
    LCD_CMDS_DISPLAY_MODE_ON_FLASH = 13,
    LCD_CMDS_DISPLAY_MODE_ON_UL_FLASH = 15,

    LCD_CMDS_SHIFT_CURSOR_LEFT = 0x10,
    LCD_CMDS_SHIFT_CURSOR_RIGHT = 0x10 | 4,
    LCD_CMDS_SHIFT_DISPLAY_LEFT = 0x10 | 8,
    LCD_CMDS_SHIFT_DISPLAY_RIGHT = 0x10 | 8 | 4,

    // 4-Bit-Mode und 2-zeilig ist vorbelegt
    LCD_CMDS_SYSTEM_SET_5X10 = 0x28 | 4, // 5x10 Zeichenbox
    LCD_CMDS_SYSTEM_SET_5X7 = 0x28, // 5x7 Zeichenbox

    LCD_CMDS_SET_CGRAM = 0x40,
    LCD_CMDS_SET_DDRAM = 0x80
};

void lcd_upper(); // obere Displayhälfte verwenden
void lcd_lower(); // untere Displayhälfte verwenden

void lcd_put ( uint8_t value, bool command ); // ein Byte zum Display schreiben
uint8_t lcd_get ( bool command ); // ein Byte vom Display lesen

void lcd_print(const char *text); // Text an die aktuelle Position schreiben
void lcd_printInt(int z);
void lcd_printHex(uint8_t z);
void lcd_printFixed(int z, uint8_t digits, uint8_t commadigits);

void lcd_set_cursor(uint8_t pos, uint8_t line); // Positioniert den Cursor auf Spalte/Zeile

void lcd_init(); // Initialisierung 15ms nach Spannungswiederkehr aufrufen

// ein paar Makros zur vereinfachung des Aufrufs:

#define lcd_put_command( command ) lcd_put( command, true )
#define lcd_put_char( character ) lcd_put( character, false )

#define lcd_clear() { lcd_put_command( LCD_CMDS_CLEAR ); }
#define lcd_home() { lcd_put_command( LCD_CMDS_HOME ); }

#define lcd_set_char_gen_addr(address) { lcd_put_command( ( address & ( LCD_CMDS_SET_CGRAM - 1 ) ) | LCD_CMDS_SET_CGRAM ); }
#define lcd_set_display_address(address) { lcd_put_command( ( address & ( LCD_CMDS_SET_DDRAM - 1 ) ) | LCD_CMDS_SET_DDRAM ); }

// Speicher für die aktuelle Cursorposition
extern volatile uint8_t lcd_pos;

// Cursor um Schrittweiter weiterschalten
void lcd_step_cursor(uint8_t step);
// Zeichen schreiben und Cursor weiterschalten
void lcd_step_char(uint8_t c);
// Print-Kommandos mit Step
void lcd_step_print(const char *text);
void lcd_step_printInt(int z);
void lcd_step_printHex(uint8_t z);
void newline();

#endif

