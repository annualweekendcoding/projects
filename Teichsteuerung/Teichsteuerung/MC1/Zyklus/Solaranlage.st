{ #include <global.st>}
{ #define addnetworks true }
{ #define addcomments true }
{ #define addcontrolcomments false }
FUNCTION_BLOCK Solaranlage
// Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.
{ #define title "Solaranlage" }
{ #define author "TeCAD" }
VAR
  Solaranlage_Op0 : BYTE; // Operation 0 aktiv
  Solaranlage_P0 : BOOL; // Aussentemperatur > 4°C
  Solaranlage_P1 : BOOL; // Solartemperatur > Teichtemperatur
  z : INT;
  Solaranlage_P2 : BOOL; // Temperatursensor Solar gestört
  Solaranlage_P3 : BOOL; // Temperatur im Teich zu gross
  Solaranlage_P4 : BOOL; // Temperatursensor Teich vorn unten gestört
END_VAR
VAR_EXTERNAL
  _INIT AT %FX(0,0) : BOOL; // Initialisierungsvariable
  Fischwasser_ein AT %QX(0,2) : BOOL; // Magnetventil Fischwasser einschalten
  time_sec AT %F(int16_t,4) : INT; // Ticks in Sekunden als Umlaufzähler
  Teich_Luft_Temp AT %I(int16_t,12) : INT; // Temperatur Luft
  Solar_Ausgang_Temp AT %I(int16_t,10) : INT; // Temperatur am Ausgang Solaranlage
  Teich_vorn_unten_Temp AT %I(int16_t,8) : INT; // Temperatur Schwimmbereich vorn unten
  Teich_Temp_max AT %R(int16_t,8) : INT; // Teichtemperatur ab der auf kühlen geschaltet wird
END_VAR

// Initialisierung der Operationsvariablen

IF NOT _INIT THEN
  Solaranlage_Op0 := 2;
END_IF;
{ #define useerrortags TRUE }

// Berechnung der nicht binären Prozeßvariablen

Solaranlage_P0 := Teich_Luft_Temp>40;
Solaranlage_P1 := Solar_Ausgang_Temp > Teich_vorn_unten_Temp;
Solaranlage_P3 := Teich_vorn_unten_Temp > Teich_Temp_max;
Solaranlage_P4 := Teich_vorn_unten_Temp<=-50;
Solaranlage_P2 := Solar_Ausgang_Temp<=-50;

// Ausgabe der Operationsübergänge

{ #define errortag "OP0" }
// Auswahl der Operation 0
IF Solaranlage_Op0 = 0 THEN
  // Übergang von Operation 0 zu Operation 1
  IF (NOT Solaranlage_P2 AND NOT Solaranlage_P4 AND Solaranlage_P0 AND ((Solaranlage_P3 AND NOT Solaranlage_P1) OR (NOT Solaranlage_P3 AND Solaranlage_P1))) THEN
    Solaranlage_Op0 := 1;
  END_IF;
{ #define errortag "OP1" }
// Auswahl der Operation 1
ELSIF Solaranlage_Op0 = 1 THEN
  // Übergang von Operation 1 zu Operation 0
  IF (Solaranlage_P2 OR Solaranlage_P4 OR NOT Solaranlage_P0 OR (Solaranlage_P3 AND Solaranlage_P1) OR (NOT Solaranlage_P3 AND NOT Solaranlage_P1)) THEN
    Solaranlage_Op0 := 0;
  END_IF;
{ #define errortag "INIT" }
ELSE
  // Initialisierung wenn keine Operation gesetzt ist
  Solaranlage_Op0 := 0;
END_IF;
{ #define useerrortags FALSE }

// Zuweisung der Operationsvariablen

// Magnetventil Fischwasser einschalten
Fischwasser_ein := (Solaranlage_Op0=1);

// Ausgabe Sonderfunktionen und Timeraufrufe

IF (Solaranlage_Op0=0) THEN
  // Zeit merken
  z := time_sec;
END_IF;

END_FUNCTION_BLOCK
