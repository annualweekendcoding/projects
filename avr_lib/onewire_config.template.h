/*******************************************/
/* Hardware connection                     */
/*******************************************/

/* Define OW_ONE_BUS if only one 1-Wire-Bus is used
   in the application -> shorter code.
   If not defined make sure to call ow_set_bus() before using 
   a bus. Runtime bus-select increases code size by around 300 
   bytes so use OW_ONE_BUS if possible */
#define OW_ONE_BUS

#ifdef OW_ONE_BUS

#define OW_PIN  PC0
#define OW_IN   PINC
#define OW_OUT  PORTC
#define OW_DDR  DDRC

#endif
