// main.h

void Initialization(void);
void OSCCAL_calibration(void);

// Macro definitions
//mtA -
// sbi and cbi are not longer supported by the avr-libc
// to avoid version-conflicts the macro-names have been
// changed to sbiBF/cbiBF "everywhere"
#define sbiBF(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbiBF(port,bit)  (port &= ~(1<<bit))  //clear bit in port
//mtE
