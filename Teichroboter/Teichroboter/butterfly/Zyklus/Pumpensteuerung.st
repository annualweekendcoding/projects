{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Pumpensteuerung
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Pumpensteuerung" }
{ #define author "TeCAD" }
VAR
  _Op0 : BYTE; // Operation 0 aktiv
  _P0 : BOOL; // Akkuspannung zu hoch
  Zeit : DINT;
  _P1 : BOOL; // Pumpzeit vergangen
  _P2 : BOOL; // Wartezeit vergangen
  _P3 : BOOL; // Kompass im Pumpbereich
  _P4 : BOOL;
END_VAR
VAR_EXTERNAL
  _INIT AT %init : BOOL; // Initialisierungsvariable
  ANLAGE_AUS AT %anlage_aus : BOOL; // Anlage AUS
  AKKU_SPANNUNG AT %akku_spannung : INT; // Spannung des Akkus
  TIME_MS AT %time_ms : DINT; // Umlaufzähler in Millisekunden
  PUMPE_EIN AT %pumpe_ein : BOOL; // Pumpe einschalten
  AKKU_NIEDRIG AT %akku_niedrig : BOOL; // Akku niedrig
  KURS AT %kurs_akt : INT; // aktuell gefahrener Kurs
  PUMPE_AUSGEKLAPPT AT %pumpe_ausgeklappt : BOOL; // Pumpe ausgeklappt
  KOMPASS_GRAD AT %kompass_grad : INT; // Kompass Richtung, 0..3599, -1 bei Fehler
END_VAR
VAR_TEMP
  _OV0 : BOOL;
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  _Op0 := 4;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

_P0 := AKKU_SPANNUNG>1420;
_P1 := (TIME_MS-Zeit)>3000;
_P2 := (TIME_MS-Zeit)>20000;
_P3 := KOMPASS_GRAD<1800 AND KOMPASS_GRAD>700;

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF _Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 1
  IF (_P0 OR (NOT AKKU_NIEDRIG AND NOT ANLAGE_AUS AND NOT PUMPE_AUSGEKLAPPT AND _P3)) THEN
    _Op0 := 1;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF _Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 0
  IF (AKKU_NIEDRIG OR ANLAGE_AUS) THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 1 zu Operation 2
  IF (NOT AKKU_NIEDRIG AND NOT ANLAGE_AUS AND _P1) THEN
    _Op0 := 2;
  END_IF;
{ #define errortag "OP2" }
// Auswahl der Operation 2
ELSIF _Op0 = 2 THEN
  // Übergang von Operation 2 zu Operation 3
  IF TRUE THEN
    _Op0 := 3;
  END_IF;
{ #define errortag "OP3" }
// Auswahl der Operation 3
ELSIF _Op0 = 3 THEN
  // Übergang von Operation 3 zu Operation 0
  IF _P2 THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  _Op0 := 0;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Pumpe einschalten
PUMPE_EIN := (_Op0=1);
// Zeit merken
_OV0 := (_Op0=0) OR (_Op0=2);

// Ausgabe Sonderfunktionen und Timeraufrufe

IF _OV0 THEN
  // Zeit merken
  Zeit := TIME_MS;
END_IF;

END_FUNCTION_BLOCK
