{ #include <global.st>}
{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Pumpensteuerung
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Pumpensteuerung" }
{ #define author "TeCAD" }
VAR
  Pumpensteuerung_Op0 : BYTE; // aktive Operationsnummer
  time_last : INT; // gemerkte Zeit
  Pumpensteuerung_P0 : BOOL; // Mindestzeit vergangen
  Pumpensteuerung_P1 : BOOL; // Einschaltzeit/10?
  Pumpensteuerung_P2 : BOOL; // Zeitüberlauf
  Pumpensteuerung_P3 : BOOL; // Temperatur an Pumpe größer als im Teich
  Pumpensteuerung_P4 : BOOL; // maximale Nachfüllzeit vergangen
  Pumpensteuerung_P5 : BOOL; // Temperatursensor Pumpe gestört
  Pumpensteuerung_P6 : BOOL; // maximale Ausschaltzeit vergangen
  Pumpensteuerung_P7 : BOOL; // Einschaltzeit/100 ?
  Pumpensteuerung_P8 : BOOL; // maximale Einschaltzeit vergangen
  Pumpensteuerung_P9 : BOOL; // maximale EIN-Zeit bei Fehltemperatur vergangen
  Pumpensteuerung_P10 : BOOL; // Temperatur im Teich zu gross
  Pumpensteuerung_P11 : BOOL; // Temperatur im Teich größer als an Pumpe
  Pumpensteuerung_P12 : BOOL; // Temperatur im Teich zu klein
  Pumpensteuerung_P13 : BOOL; // Temperatursensor Teich vorn unten gestört
END_VAR
VAR_EXTERNAL
  _INIT AT %FX(0,0) : BOOL; // Initialisierungsvariable
  time_sec AT %F(int16_t,4) : INT; // Ticks in Sekunden als Umlaufzähler
  Pumpe_EIN AT %QX(0,0) : BOOL; // Pumpe einschalten
  Pumpe_Fuellst_MIN AT %IX(0,0) : BOOL; // Füllstand Pumpenschacht unterschritten
  Filter_Fuellst_MIN AT %IX(0,1) : BOOL; // Füllstand Filtergraben unterschritten
  Pumpe_Zeit_EIN_Letzte AT %F(int16_t,10) : INT; // Letzte Einschaltzeit der Pumpe
  Pumpe_Zeit_AUS_Letzte AT %F(int16_t,12) : INT; // Letzte Auschaltzeit der Pumpe
  Pumpe_Zeit_Verh_Letzte AT %F(int16_t,14) : INT; // Letzte Einschaltdauer der Pumpe in %
  Pumpe_Zeit_Verh_Mittel AT %F(int16_t,16) : INT; // Einschaltverhältnis gemittelt in %
  Teich_vorn_unten_Temp AT %I(int16_t,8) : INT; // Temperatur Schwimmbereich vorn unten
  Pumpe_Fuellst_MIN_Stoer AT %FX(0,1) : BOOL; // Füllstandssensor Pumpenschacht gestört
  Filter_Fuellst_MIN_Stoer AT %FX(0,2) : BOOL; // Füllstandssensor Filtergraben gestört
  Teich_Pumpe_Temp AT %I(int16_t,14) : INT; // Temperatur Pumpenschacht
  time_akt AT %F(int16_t,18) : INT; // aktuelle Zeit seit letzter gemerkter Zeit
  Mindestzeit AT %R(int16_t,0) : INT; // Minimale Zeit für alle Pumpenaktionen
  Nachfuellzeit_max AT %R(int16_t,2) : INT; // Zeit bis der Min-Sensor im Filtergraben kommen muß wenn die Pumpe abgeschaltet wird
  Pumpe_Standzeit_max AT %R(int16_t,4) : INT; // Maximale Stillstandszeit der Pumpe
  Pumpe_Laufzeit_max AT %R(int16_t,6) : INT; // Maximale Laufzeit der Pumpe
  Teich_Temp_max AT %R(int16_t,8) : INT; // Teichtemperatur ab der auf kühlen geschaltet wird
  Teich_Temp_min AT %R(int16_t,10) : INT; // Teichtemperatur ab der auf heizen geschaltet wird
  Pumpe_gesperrt AT %FX(0,3) : BOOL; // Pumpe gesperrt
END_VAR
VAR_TEMP
  _OV10 : BOOL;
  _OV7 : BOOL;
  _OV6 : BOOL;
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  Pumpensteuerung_Op0 := 10;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

Pumpensteuerung_P0 := time_akt>=Mindestzeit;
Pumpensteuerung_P1 := Pumpe_Zeit_EIN_Letzte>327;
Pumpensteuerung_P2 := Pumpe_Zeit_EIN_Letzte<=0 or Pumpe_Zeit_AUS_Letzte<=0;
Pumpensteuerung_P3 := (Teich_Pumpe_Temp+2) > Teich_vorn_unten_Temp;
Pumpensteuerung_P4 := time_akt>=Nachfuellzeit_max;
Pumpensteuerung_P5 := Teich_Pumpe_Temp<=-50;
Pumpensteuerung_P6 := time_akt>=Pumpe_Standzeit_max;
Pumpensteuerung_P7 := Pumpe_Zeit_EIN_Letzte>3270;
Pumpensteuerung_P8 := time_akt>=Pumpe_Laufzeit_max;
Pumpensteuerung_P9 := time_akt>=180;
Pumpensteuerung_P10 := Teich_vorn_unten_Temp > Teich_Temp_max;
Pumpensteuerung_P11 := (Teich_vorn_unten_Temp+2) > Teich_Pumpe_Temp;
Pumpensteuerung_P12 := Teich_vorn_unten_Temp < Teich_Temp_min;
Pumpensteuerung_P13 := Teich_vorn_unten_Temp<=-50;

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF Pumpensteuerung_Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 7
  IF (NOT Pumpe_gesperrt AND Pumpensteuerung_P0 AND Pumpe_Fuellst_MIN) THEN
    Pumpensteuerung_Op0 := 7;
  END_IF;
  // Übergang von Operation 0 zu Operation 8
  IF (NOT Pumpe_gesperrt AND Pumpensteuerung_P0 AND NOT Pumpe_Fuellst_MIN AND Filter_Fuellst_MIN AND Pumpensteuerung_P4) THEN
    Pumpensteuerung_Op0 := 8;
  END_IF;
  // Übergang von Operation 0 zu Operation 3
  IF (NOT Pumpe_gesperrt AND Pumpensteuerung_P0 AND NOT Pumpe_Fuellst_MIN AND ((NOT Filter_Fuellst_MIN AND (Pumpensteuerung_P5 OR Pumpensteuerung_P13 OR (Pumpensteuerung_P10 AND Pumpensteuerung_P11) OR (NOT Pumpensteuerung_P10 AND (NOT Pumpensteuerung_P12 OR Pumpensteuerung_P3)))) OR (((Filter_Fuellst_MIN AND NOT Pumpensteuerung_P4) OR (NOT Filter_Fuellst_MIN AND NOT Pumpensteuerung_P5 AND NOT Pumpensteuerung_P13 AND ((Pumpensteuerung_P10 AND NOT Pumpensteuerung_P11) OR (NOT Pumpensteuerung_P10 AND Pumpensteuerung_P12 AND NOT Pumpensteuerung_P3)))) AND Pumpensteuerung_P6))) THEN
    Pumpensteuerung_Op0 := 3;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF Pumpensteuerung_Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 4
  IF Pumpe_gesperrt THEN
    Pumpensteuerung_Op0 := 4;
  END_IF;
  // Übergang von Operation 1 zu Operation 5
  IF (NOT Pumpe_gesperrt AND Pumpensteuerung_P0 AND (Pumpensteuerung_P8 OR Pumpe_Fuellst_MIN OR (NOT Pumpensteuerung_P5 AND NOT Pumpensteuerung_P13 AND ((Pumpensteuerung_P10 AND NOT Pumpensteuerung_P11) OR (NOT Pumpensteuerung_P10 AND Pumpensteuerung_P12 AND NOT Pumpensteuerung_P3)) AND Pumpensteuerung_P9))) THEN
    Pumpensteuerung_Op0 := 5;
  END_IF;
{ #define errortag "OP2" }
// Auswahl der Operation 2
ELSIF Pumpensteuerung_Op0 = 2 THEN
  // Übergang von Operation 2 zu Operation 0
  Pumpensteuerung_Op0 := 0;
{ #define errortag "OP3" }
// Auswahl der Operation 3
ELSIF Pumpensteuerung_Op0 = 3 THEN
  // Übergang von Operation 3 zu Operation 1
  Pumpensteuerung_Op0 := 1;
{ #define errortag "OP4" }
// Auswahl der Operation 4
ELSIF Pumpensteuerung_Op0 = 4 THEN
  // Übergang von Operation 4 zu Operation 0
  Pumpensteuerung_Op0 := 0;
{ #define errortag "OP5" }
// Auswahl der Operation 5
ELSIF Pumpensteuerung_Op0 = 5 THEN
  // Übergang von Operation 5 zu Operation 4
  IF Pumpensteuerung_P2 THEN
    Pumpensteuerung_Op0 := 4;
  END_IF;
  // Übergang von Operation 5 zu Operation 2
  IF (NOT Pumpensteuerung_P2 AND NOT Pumpensteuerung_P1) THEN
    Pumpensteuerung_Op0 := 2;
  END_IF;
  // Übergang von Operation 5 zu Operation 6
  IF (NOT Pumpensteuerung_P2 AND Pumpensteuerung_P1 AND NOT Pumpensteuerung_P7) THEN
    Pumpensteuerung_Op0 := 6;
  END_IF;
  // Übergang von Operation 5 zu Operation 9
  IF (NOT Pumpensteuerung_P2 AND Pumpensteuerung_P1 AND Pumpensteuerung_P7) THEN
    Pumpensteuerung_Op0 := 9;
  END_IF;
{ #define errortag "OP6" }
// Auswahl der Operation 6
ELSIF Pumpensteuerung_Op0 = 6 THEN
  // Übergang von Operation 6 zu Operation 0
  Pumpensteuerung_Op0 := 0;
{ #define errortag "OP7" }
// Auswahl der Operation 7
ELSIF Pumpensteuerung_Op0 = 7 THEN
  // Übergang von Operation 7 zu Operation 4
  IF NOT Pumpe_Fuellst_MIN THEN
    Pumpensteuerung_Op0 := 4;
  END_IF;
{ #define errortag "OP8" }
// Auswahl der Operation 8
ELSIF Pumpensteuerung_Op0 = 8 THEN
  // Übergang von Operation 8 zu Operation 4
  IF NOT Filter_Fuellst_MIN THEN
    Pumpensteuerung_Op0 := 4;
  END_IF;
{ #define errortag "OP9" }
// Auswahl der Operation 9
ELSIF Pumpensteuerung_Op0 = 9 THEN
  // Übergang von Operation 9 zu Operation 0
  Pumpensteuerung_Op0 := 0;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  Pumpensteuerung_Op0 := 4;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Pumpe einschalten
Pumpe_EIN := (Pumpensteuerung_Op0=1);
// Störung Sensor Pumpenschacht
Pumpe_Fuellst_MIN_Stoer := (Pumpensteuerung_Op0=7);
// Störung Sensor Filtergraben
Filter_Fuellst_MIN_Stoer := (Pumpensteuerung_Op0=8);
// Zeitdifferenz berechnen
_OV10 := (Pumpensteuerung_Op0=0) OR (Pumpensteuerung_Op0=1);
// Zeitverhältnis mitteln
_OV6 := (Pumpensteuerung_Op0=2) OR (Pumpensteuerung_Op0=6) OR (Pumpensteuerung_Op0=9);
// Zeit merken
_OV7 := (Pumpensteuerung_Op0=3) OR (Pumpensteuerung_Op0=4) OR _OV6;

// Ausgabe Sonderfunktionen und Timeraufrufe

IF (Pumpensteuerung_Op0=5) THEN
  // Einschaltzeit merken
  Pumpe_Zeit_EIN_Letzte := time_sec-time_last;
END_IF;
IF (Pumpensteuerung_Op0=3) THEN
  // Ausschaltzeit merken
  Pumpe_Zeit_AUS_Letzte := time_sec - time_last;
END_IF;
IF (Pumpensteuerung_Op0=2) THEN
  // Zeitverhältnis berechnen
  Pumpe_Zeit_Verh_Letzte := (Pumpe_Zeit_EIN_Letzte * 100) / (Pumpe_Zeit_EIN_Letzte + Pumpe_Zeit_AUS_Letzte);
END_IF;
IF (Pumpensteuerung_Op0=6) THEN
  // Zeitverhaeltnis mit Zenteln berechnen
  Pumpe_Zeit_Verh_Letzte := (Pumpe_Zeit_EIN_Letzte * 10) / (Pumpe_Zeit_EIN_Letzte/10 + Pumpe_Zeit_AUS_Letzte/10);
END_IF;
IF (Pumpensteuerung_Op0=9) THEN
  // Zeitverhaeltnis mit /100 berechnen
  Pumpe_Zeit_Verh_Letzte := Pumpe_Zeit_EIN_Letzte / (Pumpe_Zeit_EIN_Letzte/100 + Pumpe_Zeit_AUS_Letzte/100);
END_IF;
IF _OV6 THEN
  // Zeitverhältnis mitteln
  Pumpe_Zeit_Verh_Mittel := (Pumpe_Zeit_Verh_Mittel*4)/5 + Pumpe_Zeit_Verh_Letzte/5;
END_IF;
IF _OV7 THEN
  // Zeit merken
  time_last := time_sec;
END_IF;
IF _OV10 THEN
  // Zeitdifferenz berechnen
  time_akt := time_sec-time_last;
END_IF;

END_FUNCTION_BLOCK
