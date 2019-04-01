#include "teranis.h"

#if PLC_I_SIZE>0
  uint8_t Inputs[PLC_I_SIZE];
#endif

#if PLC_Q_SIZE>0
  uint8_t Outputs[PLC_Q_SIZE];
#endif

#if PLC_F_SIZE>0
  uint8_t Flags[PLC_F_SIZE];
#endif

#if PLC_R_SIZE>0
  uint8_t RFlags[PLC_F_SIZE];
#endif

// Von TERANiS generierten Code einbinden
#ifdef __cplusplus
  namespace teranis {
    #include "teranis_code.h"
  };
  using teranis::Zyklus;
#else
  #include "teranis_code.h"
#endif

// Globale Instanz, da teranis_main ein Funktionsblock ist
struct Zyklus I_Zyklus;

void loop_teranis()
{
  Zyklus(&I_Zyklus);
}

void setup_teranis()
{
  // Initialisieren der Variablen
  #if PLC_I_SIZE>0
    for (int i=0; i<PLC_I_SIZE; i++) Inputs[i]=0;
  #endif
  
  #if PLC_Q_SIZE>0
    for (int i=0; i<PLC_Q_SIZE; i++) Outputs[i]=0;
  #endif
  
  #if PLC_F_SIZE>0
    for (int i=0; i<PLC_F_SIZE; i++) Flags[i]=0;
  #endif

  #if PLC_R_SIZE>0
    for (int i=0; i<PLC_R_SIZE; i++) RFlags[i]=0;
    // TODO: Remanente Flags aus Datei lesen
  #endif
}
