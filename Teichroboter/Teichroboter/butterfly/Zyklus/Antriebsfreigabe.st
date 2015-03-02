{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Antriebsfreigabe
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Antriebsfreigabe" }
{ #define author "TeCAD" }
VAR
  _Op0 : BYTE; // Operation 0 aktiv
END_VAR
VAR_EXTERNAL
  _INIT AT %init : BOOL; // Initialisierungsvariable
  ANLAGE_AUS AT %anlage_aus : BOOL; // Anlage AUS
  AKKU_NIEDRIG AT %akku_niedrig : BOOL; // Akku niedrig
  ANTRIEBSFREIGABE AT %antriebsfreigabe : BOOL; // Antriebsfreigabe erteilt
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  _Op0 := 2;
END_IF;
{ #define useerrortags TRUE }

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF _Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 1
  IF (NOT ANLAGE_AUS AND NOT AKKU_NIEDRIG) THEN
    _Op0 := 1;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF _Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 0
  IF (ANLAGE_AUS OR AKKU_NIEDRIG) THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  _Op0 := 0;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Antriebsfreigabe erteilt
ANTRIEBSFREIGABE := (_Op0=1);

END_FUNCTION_BLOCK
