#ifndef teranis_h
#define teranis_h
#include <stdint.h>
#include "avicstd\h\avi_c_std.h"

// Define the size in Bytes for different PLC memory types
#define PLC_I_SIZE 30 // Input Bytes
#define PLC_Q_SIZE 1 // Output Bytes
#define PLC_F_SIZE 1000 // Flags Bytes

typedef struct
{
  unsigned x0 : 1;
  unsigned x1 : 1;
  unsigned x2 : 1;
  unsigned x3 : 1;
  unsigned x4 : 1;
  unsigned x5 : 1;
  unsigned x6 : 1;
  unsigned x7 : 1;
} T8Bits;

// Macro zum casten eines Typs in ein Byte-Array
#define TREF(typ,arr,idx) (*((typ*)(&(arr[idx]))))

// Macro zum Zugriff auf ein Bit in einem Byte-Array
#define XREF(arr,idx,bitnr) TREF(T8Bits,arr,idx).x##bitnr

#if PLC_I_SIZE>0
  extern uint8_t Inputs[PLC_I_SIZE];
  #define I(typ,idx) TREF(typ,Inputs,idx)
  #define IX(idx,bitnr) XREF(Inputs,idx,bitnr)
#endif

#if PLC_Q_SIZE>0
  extern uint8_t Outputs[PLC_Q_SIZE];
  #define Q(typ,idx) TREF(typ,Outputs,idx)
  #define QX(idx,bitnr) XREF(Outputs,idx,bitnr)
#endif

#if PLC_F_SIZE>0
  extern uint8_t Flags[PLC_F_SIZE];
  
  #define F(typ,idx) TREF(typ,Flags,idx)
  #define FX(idx,bitnr) XREF(Flags,idx,bitnr)
#endif

void setup_teranis();
void loop_teranis();

#endif
