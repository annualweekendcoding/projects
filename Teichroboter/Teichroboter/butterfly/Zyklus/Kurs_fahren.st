{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Kurs_fahren
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Kurs_fahren" }
{ #define author "TeCAD" }
VAR
  _Op0 : BYTE; // Operation 0 aktiv
  _P0 : BOOL; // Kursdifferenz positiv
  _P1 : BOOL; // Kursdifferenz negativ
  Zeit : DINT;
  _P2 : BOOL; // Rückwärtsfahrzeit abgelaufen
  _P3 : BOOL; // maximale Drehversuche abgelaufen
  _P4 : BOOL; // Kurs im Sollbereich
  _P5 : BOOL; // Störwartezeit abgelaufen
  Fahrzeit : DINT;
  _P6 : BOOL; // Fahrzeit zu lange
  Drehversuche_merk : INT;
END_VAR
VAR_EXTERNAL
  _INIT AT %init : BOOL; // Initialisierungsvariable
  ANLAGE_AUS AT %anlage_aus : BOOL; // Anlage AUS
  KURS AT %kurs_akt : INT; // aktuell gefahrener Kurs
  KOMPASS_GRAD AT %kompass_grad : INT; // Kompass Richtung, 0..3599, -1 bei Fehler
  KURS_DIFF AT %kurs_diff : INT; // aktuelle Kursdifferenz zum Kompass
  TIME_MS AT %time_ms : DINT; // Umlaufzähler in Millisekunden
  GESCHWINDIGKEIT AT %geschwindigkeit : INT; // aktuelle Sollgeschwindigkeit
  PUMPE_AUSGEKLAPPT AT %pumpe_ausgeklappt : BOOL; // Pumpe ausgeklappt
  AKKU_NIEDRIG AT %akku_niedrig : BOOL; // Akku niedrig
  STATUSTEXTNR AT %statustextnr : BYTE; // Nummer des Textes für die Anzeige am Display
  ANTRIEBSFREIGABE AT %antriebsfreigabe : BOOL; // Antriebsfreigabe erteilt
  DREHVERSUCHE AT %drehversuche : INT; // Anzahl Drehversuche als Umlaufzähler
END_VAR
VAR_TEMP
  _OV2 : BOOL;
  _OV13 : BOOL;
  _OV14 : BOOL;
  _OV5 : BOOL;
  _OV3 : BOOL;
  _OV0 : BOOL;
  _OV1 : BOOL;
  _OV4 : BOOL;
  _OV12 : BOOL;
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  _Op0 := 19;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

_P0 := KURS_DIFF>10;
_P1 := KURS_DIFF<-10;
_P2 := (TIME_MS-Zeit)>2000;
_P3 := (DREHVERSUCHE-Drehversuche_merk)>3;
_P4 := ABS(KURS_DIFF)<150;
_P5 := (TIME_MS-Zeit)>60000;
_P6 := (TIME_MS-Fahrzeit)>120000;

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF _Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 2
  IF NOT PUMPE_AUSGEKLAPPT THEN
    _Op0 := 2;
  END_IF;
  // Übergang von Operation 0 zu Operation 1
  IF PUMPE_AUSGEKLAPPT THEN
    _Op0 := 1;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF _Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 15
  IF (_P2 AND PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 15;
  END_IF;
  // Übergang von Operation 1 zu Operation 2
  IF (_P2 AND NOT PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 2;
  END_IF;
{ #define errortag "OP2" }
// Auswahl der Operation 2
ELSIF _Op0 = 2 THEN
  // Übergang von Operation 2 zu Operation 12
  IF NOT ANTRIEBSFREIGABE THEN
    _Op0 := 12;
  END_IF;
  // Übergang von Operation 2 zu Operation 0
  IF (ANTRIEBSFREIGABE AND PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 2 zu Operation 13
  IF (ANTRIEBSFREIGABE AND NOT PUMPE_AUSGEKLAPPT AND _P6) THEN
    _Op0 := 13;
  END_IF;
  // Übergang von Operation 2 zu Operation 5
  IF (ANTRIEBSFREIGABE AND NOT PUMPE_AUSGEKLAPPT AND NOT _P6 AND _P4) THEN
    _Op0 := 5;
  END_IF;
  // Übergang von Operation 2 zu Operation 3
  IF (ANTRIEBSFREIGABE AND NOT PUMPE_AUSGEKLAPPT AND NOT _P6 AND NOT _P4 AND _P3) THEN
    _Op0 := 3;
  END_IF;
{ #define errortag "OP3" }
// Auswahl der Operation 3
ELSIF _Op0 = 3 THEN
  // Übergang von Operation 3 zu Operation 4
  IF TRUE THEN
    _Op0 := 4;
  END_IF;
{ #define errortag "OP4" }
// Auswahl der Operation 4
ELSIF _Op0 = 4 THEN
  // Übergang von Operation 4 zu Operation 12
  IF NOT ANTRIEBSFREIGABE THEN
    _Op0 := 12;
  END_IF;
  // Übergang von Operation 4 zu Operation 13
  IF (ANTRIEBSFREIGABE AND _P6) THEN
    _Op0 := 13;
  END_IF;
  // Übergang von Operation 4 zu Operation 5
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND _P4) THEN
    _Op0 := 5;
  END_IF;
  // Übergang von Operation 4 zu Operation 0
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND NOT _P4 AND _P3) THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "OP5" }
// Auswahl der Operation 5
ELSIF _Op0 = 5 THEN
  // Übergang von Operation 5 zu Operation 12
  IF NOT ANTRIEBSFREIGABE THEN
    _Op0 := 12;
  END_IF;
  // Übergang von Operation 5 zu Operation 13
  IF (ANTRIEBSFREIGABE AND _P6) THEN
    _Op0 := 13;
  END_IF;
  // Übergang von Operation 5 zu Operation 0
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND NOT _P4) THEN
    _Op0 := 0;
  END_IF;
  // Übergang von Operation 5 zu Operation 6
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND _P4 AND PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 6;
  END_IF;
{ #define errortag "OP6" }
// Auswahl der Operation 6
ELSIF _Op0 = 6 THEN
  // Übergang von Operation 6 zu Operation 8
  IF NOT PUMPE_AUSGEKLAPPT THEN
    _Op0 := 8;
  END_IF;
  // Übergang von Operation 6 zu Operation 7
  IF PUMPE_AUSGEKLAPPT THEN
    _Op0 := 7;
  END_IF;
{ #define errortag "OP7" }
// Auswahl der Operation 7
ELSIF _Op0 = 7 THEN
  // Übergang von Operation 7 zu Operation 17
  IF (_P2 AND PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 17;
  END_IF;
  // Übergang von Operation 7 zu Operation 8
  IF (_P2 AND NOT PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 8;
  END_IF;
{ #define errortag "OP8" }
// Auswahl der Operation 8
ELSIF _Op0 = 8 THEN
  // Übergang von Operation 8 zu Operation 12
  IF NOT ANTRIEBSFREIGABE THEN
    _Op0 := 12;
  END_IF;
  // Übergang von Operation 8 zu Operation 6
  IF (ANTRIEBSFREIGABE AND PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 6;
  END_IF;
  // Übergang von Operation 8 zu Operation 14
  IF (ANTRIEBSFREIGABE AND NOT PUMPE_AUSGEKLAPPT AND _P6) THEN
    _Op0 := 14;
  END_IF;
  // Übergang von Operation 8 zu Operation 11
  IF (ANTRIEBSFREIGABE AND NOT PUMPE_AUSGEKLAPPT AND NOT _P6 AND _P4) THEN
    _Op0 := 11;
  END_IF;
  // Übergang von Operation 8 zu Operation 9
  IF (ANTRIEBSFREIGABE AND NOT PUMPE_AUSGEKLAPPT AND NOT _P6 AND NOT _P4 AND _P3) THEN
    _Op0 := 9;
  END_IF;
{ #define errortag "OP9" }
// Auswahl der Operation 9
ELSIF _Op0 = 9 THEN
  // Übergang von Operation 9 zu Operation 10
  IF TRUE THEN
    _Op0 := 10;
  END_IF;
{ #define errortag "OP10" }
// Auswahl der Operation 10
ELSIF _Op0 = 10 THEN
  // Übergang von Operation 10 zu Operation 12
  IF NOT ANTRIEBSFREIGABE THEN
    _Op0 := 12;
  END_IF;
  // Übergang von Operation 10 zu Operation 14
  IF (ANTRIEBSFREIGABE AND _P6) THEN
    _Op0 := 14;
  END_IF;
  // Übergang von Operation 10 zu Operation 11
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND _P4) THEN
    _Op0 := 11;
  END_IF;
  // Übergang von Operation 10 zu Operation 6
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND NOT _P4 AND _P3) THEN
    _Op0 := 6;
  END_IF;
{ #define errortag "OP11" }
// Auswahl der Operation 11
ELSIF _Op0 = 11 THEN
  // Übergang von Operation 11 zu Operation 12
  IF NOT ANTRIEBSFREIGABE THEN
    _Op0 := 12;
  END_IF;
  // Übergang von Operation 11 zu Operation 14
  IF (ANTRIEBSFREIGABE AND _P6) THEN
    _Op0 := 14;
  END_IF;
  // Übergang von Operation 11 zu Operation 6
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND NOT _P4) THEN
    _Op0 := 6;
  END_IF;
  // Übergang von Operation 11 zu Operation 0
  IF (ANTRIEBSFREIGABE AND NOT _P6 AND _P4 AND PUMPE_AUSGEKLAPPT) THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "OP12" }
// Auswahl der Operation 12
ELSIF _Op0 = 12 THEN
  // Übergang von Operation 12 zu Operation 5
  IF ANTRIEBSFREIGABE THEN
    _Op0 := 5;
  END_IF;
{ #define errortag "OP13" }
// Auswahl der Operation 13
ELSIF _Op0 = 13 THEN
  // Übergang von Operation 13 zu Operation 6
  IF TRUE THEN
    _Op0 := 6;
  END_IF;
{ #define errortag "OP14" }
// Auswahl der Operation 14
ELSIF _Op0 = 14 THEN
  // Übergang von Operation 14 zu Operation 0
  IF TRUE THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "OP15" }
// Auswahl der Operation 15
ELSIF _Op0 = 15 THEN
  // Übergang von Operation 15 zu Operation 16
  IF TRUE THEN
    _Op0 := 16;
  END_IF;
{ #define errortag "OP16" }
// Auswahl der Operation 16
ELSIF _Op0 = 16 THEN
  // Übergang von Operation 16 zu Operation 0
  IF _P2 THEN
    _Op0 := 0;
  END_IF;
{ #define errortag "OP17" }
// Auswahl der Operation 17
ELSIF _Op0 = 17 THEN
  // Übergang von Operation 17 zu Operation 18
  IF TRUE THEN
    _Op0 := 18;
  END_IF;
{ #define errortag "OP18" }
// Auswahl der Operation 18
ELSIF _Op0 = 18 THEN
  // Übergang von Operation 18 zu Operation 6
  IF _P2 THEN
    _Op0 := 6;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  _Op0 := 12;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Kurs nach vorn
_OV4 := (_Op0=8) OR (_Op0=11);
// Geschwindigkeit = vorwärts
_OV1 := (_Op0=5) OR (_Op0=11);
// Kurs nach hinten
_OV0 := (_Op0=2) OR (_Op0=5);
// Fahrzeit merken
_OV12 := (_Op0=12) OR (_Op0=13) OR (_Op0=14);
// Geschwindigkeit = rückwärts
_OV5 := (_Op0=1) OR (_Op0=4) OR (_Op0=7) OR (_Op0=10);
// Drehversuche merken
_OV14 := (_Op0=0) OR (_Op0=3) OR (_Op0=6) OR (_Op0=9);
// Geschwindigkeit=0
_OV3 := (_Op0=2) OR (_Op0=8) OR (_Op0=12) OR (_Op0=15) OR (_Op0=17);
// Kurs = Kompass
_OV13 := (_Op0=0) OR (_Op0=6) OR (_Op0=12) OR (_Op0=15) OR (_Op0=17);
// Zeit merken
_OV2 := (_Op0=5) OR (_Op0=11) OR _OV13;

// Ausgabe Sonderfunktionen und Timeraufrufe

IF _OV0 THEN
  // Kurs nach hinten
  KURS := 2800;
END_IF;
IF _OV1 THEN
  // Geschwindigkeit = vorwärts
  GESCHWINDIGKEIT := 100;
END_IF;
IF _OV2 THEN
  // Zeit merken
  Zeit := TIME_MS;
END_IF;
IF _OV3 THEN
  // Geschwindigkeit=0
  GESCHWINDIGKEIT := 0;
END_IF;
IF _OV4 THEN
  // Kurs nach vorn
  KURS := 1300;
END_IF;
IF _OV5 THEN
  // Geschwindigkeit = rückwärts
  GESCHWINDIGKEIT := -100;
END_IF;
IF (_Op0=12) THEN
  // Statustext AUS
  STATUSTEXTNR := 1;
END_IF;
IF (_Op0=0) THEN
  // Statustext DRHINT
  STATUSTEXTNR := 2;
END_IF;
IF (_Op0=5) THEN
  // Statustext FAHINT
  STATUSTEXTNR := 3;
END_IF;
IF (_Op0=6) THEN
  // Statustext DRVORN
  STATUSTEXTNR := 4;
END_IF;
IF (_Op0=11) THEN
  // Statustext FAVORN
  STATUSTEXTNR := 5;
END_IF;
IF _OV12 THEN
  // Fahrzeit merken
  Fahrzeit := TIME_MS;
END_IF;
IF _OV13 THEN
  // Kurs = Kompass
  KURS := KOMPASS_GRAD;
END_IF;
IF _OV14 THEN
  // Drehversuche merken
  Drehversuche_merk := DREHVERSUCHE;
END_IF;

END_FUNCTION_BLOCK
