
struct Motorsteuerung {
/* VAR */
/* Operation 0 aktiv*/
BYTE _Op0;
/* Kursdifferenz zu groß*/
BOOL _P0;
/* Kursdifferenz zu klein*/
BOOL _P1;
DINT Zeit;
/* Geschwindigkeit = 0*/
BOOL _P2;
/* Drehimpuls abgelaufen*/
BOOL _P3;
/* Geschwindigkeit > 0*/
BOOL _P4;
/* Pausenzeit abgelaufen*/
BOOL _P5;
INT Impulsdauer;
/* Kleiner Drehimpuls abgelaufen*/
BOOL _P6;
};

VOID Motorsteuerung(struct Motorsteuerung *inst)
{
/* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/

/* VAR_TEMP */
BOOL _OV0;
BOOL _OV3;
BOOL _OV4;
BOOL _OV9;
BOOL _OV8;
BOOL _OV7;

/* Titel: Motorsteuerung */
/* Author: TeCAD */

/* Initialisierung der Operationsvariablen */
if (!init) {
  (*inst)._Op0 = 10;
}
/* Berechnung der nicht binären Prozeßvariablen */
(*inst)._P0 = kurs_diff > 150;
(*inst)._P1 = kurs_diff < -150;
(*inst)._P3 = (time_ms - (*inst).Zeit) > INT_TO_DINT((*inst).Impulsdauer);
(*inst)._P2 = geschwindigkeit < 0;
(*inst)._P5 = (time_ms - (*inst).Zeit) > 3000;
(*inst)._P4 = geschwindigkeit > 0;
(*inst)._P6 = (time_ms - (*inst).Zeit) > INT_TO_DINT(((*inst).Impulsdauer / 5));
/* Ausgabe der Operationsübergänge */
/* Auswahl der Operation 0 */
if ((*inst)._Op0 == 0) {
/* Übergang von Operation 0 zu Operation 4 */
  if (((*inst)._P0 && (*inst)._P2)) {
    (*inst)._Op0 = 4;
  }
/* Übergang von Operation 0 zu Operation 3 */
  if (((*inst)._P0 && !(*inst)._P2)) {
    (*inst)._Op0 = 3;
  }
/* Übergang von Operation 0 zu Operation 2 */
  if ((!(*inst)._P0 && (*inst)._P1 && (*inst)._P2)) {
    (*inst)._Op0 = 2;
  }
/* Übergang von Operation 0 zu Operation 1 */
  if ((!(*inst)._P0 && (*inst)._P1 && !(*inst)._P2)) {
    (*inst)._Op0 = 1;
  }
/* Übergang von Operation 0 zu Operation 5 */
  if ((!(*inst)._P0 && !(*inst)._P1 && ((*inst)._P2 || (*inst)._P4))) {
    (*inst)._Op0 = 5;
  }
}
/* Auswahl der Operation 1 */
else if ((*inst)._Op0 == 1) {
/* Übergang von Operation 1 zu Operation 0 */
  if (!(*inst)._P1) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 1 zu Operation 6 */
  if (((*inst)._P1 && (*inst)._P3)) {
    (*inst)._Op0 = 6;
  }
}
/* Auswahl der Operation 2 */
else if ((*inst)._Op0 == 2) {
/* Übergang von Operation 2 zu Operation 0 */
  if (!(*inst)._P1) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 2 zu Operation 8 */
  if (((*inst)._P1 && (*inst)._P6)) {
    (*inst)._Op0 = 8;
  }
}
/* Auswahl der Operation 3 */
else if ((*inst)._Op0 == 3) {
/* Übergang von Operation 3 zu Operation 0 */
  if (!(*inst)._P0) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 3 zu Operation 6 */
  if (((*inst)._P0 && (*inst)._P3)) {
    (*inst)._Op0 = 6;
  }
}
/* Auswahl der Operation 4 */
else if ((*inst)._Op0 == 4) {
/* Übergang von Operation 4 zu Operation 0 */
  if (!(*inst)._P0) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 4 zu Operation 9 */
  if (((*inst)._P0 && (*inst)._P6)) {
    (*inst)._Op0 = 9;
  }
}
/* Auswahl der Operation 5 */
else if ((*inst)._Op0 == 5) {
/* Übergang von Operation 5 zu Operation 4 */
  if (((*inst)._P0 && (*inst)._P2)) {
    (*inst)._Op0 = 4;
  }
/* Übergang von Operation 5 zu Operation 3 */
  if (((*inst)._P0 && !(*inst)._P2)) {
    (*inst)._Op0 = 3;
  }
/* Übergang von Operation 5 zu Operation 2 */
  if ((!(*inst)._P0 && (*inst)._P1 && (*inst)._P2)) {
    (*inst)._Op0 = 2;
  }
/* Übergang von Operation 5 zu Operation 1 */
  if ((!(*inst)._P0 && (*inst)._P1 && !(*inst)._P2)) {
    (*inst)._Op0 = 1;
  }
/* Übergang von Operation 5 zu Operation 0 */
  if ((!(*inst)._P0 && !(*inst)._P1 && !(*inst)._P2 && !(*inst)._P4)) {
    (*inst)._Op0 = 0;
  }
}
/* Auswahl der Operation 6 */
else if ((*inst)._Op0 == 6) {
/* Übergang von Operation 6 zu Operation 7 */
  if (TRUE) {
    (*inst)._Op0 = 7;
  }
}
/* Auswahl der Operation 7 */
else if ((*inst)._Op0 == 7) {
/* Übergang von Operation 7 zu Operation 0 */
  if ((*inst)._P5) {
    (*inst)._Op0 = 0;
  }
}
/* Auswahl der Operation 8 */
else if ((*inst)._Op0 == 8) {
/* Übergang von Operation 8 zu Operation 1 */
  if (TRUE) {
    (*inst)._Op0 = 1;
  }
}
/* Auswahl der Operation 9 */
else if ((*inst)._Op0 == 9) {
/* Übergang von Operation 9 zu Operation 3 */
  if (TRUE) {
    (*inst)._Op0 = 3;
  }
}
else {
/* Initialisierung wenn keine Operation gesetzt ist */
  (*inst)._Op0 = 0;
}
/* Zuweisung der Operationsvariablen */
/* Rechts negativ */
_OV7 = ((*inst)._Op0 == 3) || ((*inst)._Op0 == 4);
/* Links negativ */
_OV8 = ((*inst)._Op0 == 1) || ((*inst)._Op0 == 2);
/* Impulsdauer berechnen */
_OV9 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 5);
/* Links aus */
_OV4 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 7) || ((*inst)._Op0 == 8);
/* Rechts aus */
_OV3 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 7) || ((*inst)._Op0 == 9);
/* Zeit merken */
_OV0 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 6) || ((*inst)._Op0 == 8) || ((*inst)._Op0 == 9);
/* Ausgabe Sonderfunktionen und Timeraufrufe */
if (_OV0) {
/* Zeit merken */
  (*inst).Zeit = time_ms;
}
if (((*inst)._Op0 == 1)) {
/* Rechts positiv */
  rechts_pwm = 100;
}
if (((*inst)._Op0 == 3)) {
/* Links positiv */
  links_pwm = 100;
}
if (_OV3) {
/* Rechts aus */
  rechts_pwm = 0;
}
if (_OV4) {
/* Links aus */
  links_pwm = 0;
}
if (((*inst)._Op0 == 5)) {
/* Links Geschwindigkeit */
  links_pwm = geschwindigkeit;
}
if (((*inst)._Op0 == 5)) {
/* Rechts Geschwindigkeit */
  rechts_pwm = geschwindigkeit;
}
if (_OV7) {
/* Rechts negativ */
  rechts_pwm = -120;
}
if (_OV8) {
/* Links negativ */
  links_pwm = -120;
}
if (_OV9) {
/* Impulsdauer berechnen */
  (*inst).Impulsdauer = ABS(kurs_diff) * 5;
}
if (((*inst)._Op0 == 6)) {
/* Drehversuche erhöhen */
  drehversuche = drehversuche + 1;
}
}
