// Konfiguration der LED-Ausg�nge

// Folgende LED-Namen werden unterst�tzt:
/*
 LED
 LED1-LED9
 RED
 GREEN
 YELLOW
*/

// andere Namen k�nnen benutzt werden, es muss dann aber LED_INIT(NAME) aufgerufen werden.

#define RED_DDR DDRB
#define RED_PORT PORTB
#define RED_BIT 1
#define RED_FLASHTIME 50

#define GREEN_DDR DDRC
#define GREEN_PORT PORTC
#define GREEN_BIT 1
#define GREEN_FLASHTIME 50
