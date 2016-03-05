{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Motorsteuerung
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Motorsteuerung" }
{ #define author "TeCAD" }
VAR
  _Op0 : BYTE; // Operation 0 aktiv
  _P0 : BOOL; // Kursdifferenz zu groß
  _P1 : BOOL; // Kursdifferenz zu klein
  Zeit : DINT;
  _P2 : BOOL; // Geschwindigkeit = 0
  _P3 : BOOL; // Drehimpuls abgelaufen
  _P4 : BOOL; // Geschwindigkeit > 0
  _P5 : BOOL; // Pausenzeit abgelaufen
  Impulsdauer : INT;
  _P6 : BOOL; // Kleiner Drehimpuls abgelaufen
END_VAR
VAR_EXTERNAL
  _INIT AT %init : BOOL; // Initialisierungsvariable
  RECHTS_PWM AT %rechts_pwm : INT; // Geschwindigkeit Motor rechts
  LINKS_PWM AT %links_pwm : INT; // Geschwindigkeit Motor links
  KURS_DIFF AT %kurs_diff : INT; // aktuelle Kursdifferenz zum Kompass
  GESCHWINDIGKEIT AT %geschwindigkeit : INT; // aktuelle Sollgeschwindigkeit
  TIME_MS AT %time_ms : DINT; // Umlaufzähler in Millisekunden
  DREHVERSUCHE AT %drehversuche : INT; // Anzahl Drehversuche als Umlaufzähler
END_VAR
VAR_TEMP
  _OV0 : BOOL;
  _OV3 : BOOL;
  _OV4 : BOOL;
  _OV9 : BOOL;
  _OV8 : BOOL;
  _OV7 : BOOL;
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  _Op0 := 10;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

_P0 := KURS_DIFF > 150;
_P1 := KURS_DIFF < -150;
_P3 := (TIME_MS-Zeit)>Impulsdauer;
_P2 := GESCHWINDIGKEIT<0;
_P5 := (TIME_MS-Zeit)>3000;
_P4 := GESCHWINDIGKEIT>0;
_P6 := (TIME_MS-Zeit)>(Impulsdauer/5);

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF _Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 4
  IF (_P0 AND _P2) THEN
    _Op0 := 4;
  END_IF;
  // Übergang von Operation 0 zu Operation 3
  IF (_P0 AND NOT _P2) THEN
    _Op0 := 3;
  END_IF;
  // Übergang von Operation 0 zu Operation 2
  IF (NOT _P0 AND _P1 AND _P2) THEN
    _Op0 := 2;
  END_IF;
  // Übergang von Operation 0 zu Operation 1
  IF (NOT _P0 AND _P1 AND NOT _P2) THEN
    _Op0 := 1;
  END_IF;
  // Übergang von Operation 0 zu Operation 5
  IF (NOT _P0 AND NOT _P1 AND (_P2 OR _P4)) THEN
    _Op0 := 5;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF _Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 0
  IF NOT _P1 THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 1 zu Operation 6
  IF (_P1 AND _P3) THEN
    _Op0 := 6;
  END_IF;
{ #define errortag "OP2" }
// Auswahl der Operation 2
ELSIF _Op0 = 2 THEN
  // Übergang von Operation 2 zu Operation 0
  IF NOT _P1 THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 2 zu Operation 8
  IF (_P1 AND _P6) THEN
    _Op0 := 8;
  END_IF;
{ #define errortag "OP3" }
// Auswahl der Operation 3
ELSIF _Op0 = 3 THEN
  // Übergang von Operation 3 zu Operation 0
  IF NOT _P0 THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 3 zu Operation 6
  IF (_P0 AND _P3) THEN
    _Op0 := 6;
  END_IF;
{ #define errortag "OP4" }
// Auswahl der Operation 4
ELSIF _Op0 = 4 THEN
  // Übergang von Operation 4 zu Operation 0
  IF NOT _P0 THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 4 zu Operation 9
  IF (_P0 AND _P6) THEN
    _Op0 := 9;
  END_IF;
{ #define errortag "OP5" }
// Auswahl der Operation 5
ELSIF _Op0 = 5 THEN
  // Übergang von Operation 5 zu Operation 4
  IF (_P0 AND _P2) THEN
    _Op0 := 4;
  END_IF;
  // Übergang von Operation 5 zu Operation 3
  IF (_P0 AND NOT _P2) THEN
    _Op0 := 3;
  END_IF;
  // Übergang von Operation 5 zu Operation 2
  IF (NOT _P0 AND _P1 AND _P2) THEN
    _Op0 := 2;
  END_IF;
  // Übergang von Operation 5 zu Operation 1
  IF (NOT _P0 AND _P1 AND NOT _P2) THEN
    _Op0 := 1;
  END_IF;
  // Übergang von Operation 5 zu Operation 0
  IF (NOT _P0 AND NOT _P1 AND NOT _P2 AND NOT _P4) THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "OP6" }
// Auswahl der Operation 6
ELSIF _Op0 = 6 THEN
  // Übergang von Operation 6 zu Operation 7
  IF TRUE THEN
    _Op0 := 7;
  END_IF;
{ #define errortag "OP7" }
// Auswahl der Operation 7
ELSIF _Op0 = 7 THEN
  // Übergang von Operation 7 zu Operation 0
  IF _P5 THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "OP8" }
// Auswahl der Operation 8
ELSIF _Op0 = 8 THEN
  // Übergang von Operation 8 zu Operation 1
  IF TRUE THEN
    _Op0 := 1;
  END_IF;
{ #define errortag "OP9" }
// Auswahl der Operation 9
ELSIF _Op0 = 9 THEN
  // Übergang von Operation 9 zu Operation 3
  IF TRUE THEN
    _Op0 := 3;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  _Op0 := 0;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Rechts negativ
_OV7 := (_Op0=3) OR (_Op0=4);
// Links negativ
_OV8 := (_Op0=1) OR (_Op0=2);
// Impulsdauer berechnen
_OV9 := (_Op0=0) OR (_Op0=5);
// Links aus
_OV4 := (_Op0=0) OR (_Op0=7) OR (_Op0=8);
// Rechts aus
_OV3 := (_Op0=0) OR (_Op0=7) OR (_Op0=9);
// Zeit merken
_OV0 := (_Op0=0) OR (_Op0=6) OR (_Op0=8) OR (_Op0=9);

// Ausgabe Sonderfunktionen und Timeraufrufe

IF _OV0 THEN
  // Zeit merken
  Zeit := TIME_MS;
END_IF;
IF (_Op0=1) THEN
  // Rechts positiv
  RECHTS_PWM := 100;
END_IF;
IF (_Op0=3) THEN
  // Links positiv
  LINKS_PWM := 100;
END_IF;
IF _OV3 THEN
  // Rechts aus
  RECHTS_PWM := 0;
END_IF;
IF _OV4 THEN
  // Links aus
  LINKS_PWM := 0;
END_IF;
IF (_Op0=5) THEN
  // Links Geschwindigkeit
  LINKS_PWM := GESCHWINDIGKEIT;
END_IF;
IF (_Op0=5) THEN
  // Rechts Geschwindigkeit
  RECHTS_PWM := GESCHWINDIGKEIT;
END_IF;
IF _OV7 THEN
  // Rechts negativ
  RECHTS_PWM := -120;
END_IF;
IF _OV8 THEN
  // Links negativ
  LINKS_PWM := -120;
END_IF;
IF _OV9 THEN
  // Impulsdauer berechnen
  Impulsdauer := ABS(KURS_DIFF)*5;
END_IF;
IF (_Op0=6) THEN
  // Drehversuche erhöhen
  DREHVERSUCHE := DREHVERSUCHE+1;
END_IF;

END_FUNCTION_BLOCK
