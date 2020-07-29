{ #include <global.st>}
{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION Temperatursensoren : VOID
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Temperatursensoren" }
{ #define author "TeCAD" }
VAR
  Temperatursensoren_Op0 AT %F(33) : BYTE; // Operation 0 aktiv
  Temperatursensoren_P0 AT %FX(34,0) : BOOL; // 10s vergangen
  Temperatursensoren_P1 AT %FX(34,1) : BOOL; // weitere 10ms vergangen
  zeit_merk AT %F(8) : INT; // gemerkte Zeit
  diff AT %F(38) : INT;
END_VAR
VAR_EXTERNAL
  _INIT AT %FX(0,0) : BOOL; // Initialisierungsvariable
  Temperaturen_lesen AT %QX(0,1) : BOOL; // Temperaturen vom OneWire einlesen
  Temperaturen_einschalten AT %QX(0,6) : BOOL; // Spannung für Temperatursensoren einschalten
  time_ms AT %F(2) : INT; // Ticks in Sekunden als Umlaufzähler
END_VAR
VAR_TEMP
  _OV3 : BOOL;
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  Temperatursensoren_Op0 := 4;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

Temperatursensoren_P0 := diff>10000;
Temperatursensoren_P1 := diff>10010;

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF Temperatursensoren_Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 1
  Temperatursensoren_Op0 := 1;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF Temperatursensoren_Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 3
  IF Temperatursensoren_P0 THEN
    Temperatursensoren_Op0 := 3;
  END_IF;
{ #define errortag "OP2" }
// Auswahl der Operation 2
ELSIF Temperatursensoren_Op0 = 2 THEN
  // Übergang von Operation 2 zu Operation 0
  Temperatursensoren_Op0 := 0;
{ #define errortag "OP3" }
// Auswahl der Operation 3
ELSIF Temperatursensoren_Op0 = 3 THEN
  // Übergang von Operation 3 zu Operation 2
  IF Temperatursensoren_P1 THEN
    Temperatursensoren_Op0 := 2;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  Temperatursensoren_Op0 := 0;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Temperaturen vom OneWire einlesen
Temperaturen_lesen := (Temperatursensoren_Op0=2);
// Spannung für Temperatursensoren einschalten
Temperaturen_einschalten := (Temperatursensoren_Op0=2) OR (Temperatursensoren_Op0=3);
// Differenz berechnen
_OV3 := (Temperatursensoren_Op0=1) OR (Temperatursensoren_Op0=3);

// Ausgabe Sonderfunktionen und Timeraufrufe

IF (Temperatursensoren_Op0=0) THEN
  // Zeit merken
  zeit_merk := time_ms;
END_IF;
IF _OV3 THEN
  // Differenz berechnen
  diff := time_ms-zeit_merk;
END_IF;

END_FUNCTION
