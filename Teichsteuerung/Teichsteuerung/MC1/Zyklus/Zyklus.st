{ #use "akttimes.st" }
{ #use "Pumpensteuerung.st" }
{ #use "Solaranlage.st" }
{ #use "Temperatursensoren.st" }
FUNCTION_BLOCK Zyklus
// automatisch generiert von TERANiS-Studio, bitte Änderungen nur dort vornehmen.
{ #define title "Zyklus" }
{ #define author "TERANiS" }
VAR
  I_Pumpensteuerung : Pumpensteuerung;
  I_Solaranlage : Solaranlage;
  I_Temperatursensoren : Temperatursensoren;
END_VAR
VAR_EXTERNAL
  _INIT AT %FX(0,0) : BOOL;
END_VAR
  akttimes(); // akttimes.st
  I_Pumpensteuerung(); // Pumpensteuerung.prp
  I_Solaranlage(); // Solaranlage.prp
  I_Temperatursensoren(); // Temperatursensoren.prp
  // Initialisierungsvariable setzen
  _INIT := TRUE;
END_FUNCTION_BLOCK
