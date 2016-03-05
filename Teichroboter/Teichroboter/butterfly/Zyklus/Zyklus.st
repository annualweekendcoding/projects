{ #use "Akkustand.st" }
{ #use "Antriebsfreigabe.st" }
{ #use "Kurs_fahren.st" }
{ #use "Kursdifferenz.st" }
{ #use "Motorsteuerung.st" }
{ #use "Pumpensteuerung.st" }
FUNCTION_BLOCK Zyklus
// automatisch generiert von TERANiS-Studio, bitte Änderungen nur dort vornehmen.
{ #define title "Zyklus" }
{ #define author "TERANiS" }
VAR
  I_Akkustand : Akkustand;
  I_Antriebsfreigabe : Antriebsfreigabe;
  I_Kurs_fahren : Kurs_fahren;
  I_Motorsteuerung : Motorsteuerung;
  I_Pumpensteuerung : Pumpensteuerung;
  _INIT AT %init : BOOL;
END_VAR
  I_Akkustand(); // Akkustand.prp
  I_Antriebsfreigabe(); // Antriebsfreigabe.prp
  I_Kurs_fahren(); // Kurs_fahren.prp
  Kursdifferenz(); // Kursdifferenz.st
  I_Motorsteuerung(); // Motorsteuerung.prp
  I_Pumpensteuerung(); // Pumpensteuerung.prp
  // Initialisierungsvariable setzen
  _INIT := TRUE;
END_FUNCTION_BLOCK
