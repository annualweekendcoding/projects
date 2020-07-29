{ #use "akttimes.st" }
{ #use "Pumpensteuerung.st" }
{ #use "Solaranlage.st" }
{ #use "Temperatursensoren.st" }
FUNCTION Zyklus : VOID
// automatisch generiert von TERANiS-Studio, bitte Änderungen nur dort vornehmen.
{ #define title "Zyklus" }
{ #define author "TERANiS" }
VAR_EXTERNAL
  _INIT AT %FX(0,0) : BOOL;
END_VAR
  akttimes(); // akttimes.st
  Pumpensteuerung(); // Pumpensteuerung.prp
  Solaranlage(); // Solaranlage.prp
  Temperatursensoren(); // Temperatursensoren.prp
  // Initialisierungsvariable setzen
  _INIT := TRUE;
END_FUNCTION
