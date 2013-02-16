#ifndef tbyte_h
#define tbyte_h

// Typdefinition um über Struktur auf einzelne Bits eines Bytes zugreifen zu können
// Sie können benutzt werden um das Prozessabbild darzustellen 
// oder um bei der Speicherung von BOOL-Werten Speicher zu sparen

typedef struct T8Bits
{
  unsigned bit0 : 1;  
  unsigned bit1 : 1;  
  unsigned bit2 : 1;  
  unsigned bit3 : 1;  
  unsigned bit4 : 1;  
  unsigned bit5 : 1;  
  unsigned bit6 : 1;  
  unsigned bit7 : 1;  
} T8Bits;

typedef union TByte
{
  T8Bits bits;
  uint8_t byte;
} TByte;

#endif
