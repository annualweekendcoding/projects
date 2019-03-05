#ifndef teranis_h
#define teranis_h
#include <stdint.h>
#include "avicstd\h\avi_c_std.h"

// Define the size in Bytes for different PLC memory types
#define PLC_I_SIZE 30 // Input Bytes
#define PLC_Q_SIZE 1 // Output Bytes
#define PLC_F_SIZE 68 // Flags Bytes
#define PLC_R_SIZE 10 // Remanent Flags Bytes

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
#ifdef __cplusplus
  // Templatefunktion zum Ermitteln eines Zeigers mit definiertem Typ in ein Variablenfeld
  // Dabei wird auf Überschreitung des Index getestet und zur Übersetzungszeit Fehler gemeldet
  template <typename T,const unsigned S,const unsigned I>
  T* TeArrayElement(uint8_t *A)
  {
    static_assert(S>=(I+sizeof(T)),"index and size of type exceeds size of array");
    return ((T*)(&(A[I])));
  }; 

  // Macrodefinition
  #define TREF(typ,arr,idx) (*TeArrayElement<typ,sizeof(Flags),idx>(Flags))   
#else
  // Die Standard-C Variante ohne Indexüberprüfung
  #define TREF(typ,arr,idx) (*((typ*)(&(arr[idx]))))
#endif

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
  #undef F
  #define F(typ,idx) TREF(typ,Flags,idx)
  #define FX(idx,bitnr) XREF(Flags,idx,bitnr)
#endif

#if PLC_R_SIZE>0
  extern uint8_t RFlags[PLC_F_SIZE];
  #define R(typ,idx) TREF(typ,RFlags,idx)
  #define RX(idx,bitnr) XREF(RFlags,idx,bitnr)
#endif

void setup_teranis();
void loop_teranis();

#endif
