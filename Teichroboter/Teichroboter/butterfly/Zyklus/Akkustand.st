{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Akkustand
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Akkustand" }
{ #define author "TeCAD" }
VAR
  _Op0 : BYTE; // Operation 0 aktiv
  _P0 : BOOL; // Akku unter Minimum
  _P1 : BOOL; // Akku über Wiedereinschalten
END_VAR
VAR_EXTERNAL
  AKKU_NIEDRIG AT %akku_niedrig : BOOL; // Akku niedrig
  AKKU_SPANNUNG AT %akku_spannung : INT; // Spannung des Akkus
  _INIT AT %init : BOOL; // Initialisierungsvariable
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  _Op0 := 2;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

_P0 := AKKU_SPANNUNG<1000;
_P1 := AKKU_SPANNUNG>1200;

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF _Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 1
  IF _P0 THEN
    _Op0 := 1;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF _Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 0
  IF (_P1 AND NOT _P0) THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  _Op0 := 0;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Akku niedrig
AKKU_NIEDRIG := (_Op0=1);

END_FUNCTION_BLOCK
