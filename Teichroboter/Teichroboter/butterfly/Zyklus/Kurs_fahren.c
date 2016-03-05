
struct Kurs_fahren {
/* VAR */
/* Operation 0 aktiv*/
BYTE _Op0;
/* Kursdifferenz positiv*/
BOOL _P0;
/* Kursdifferenz negativ*/
BOOL _P1;
DINT Zeit;
/* Rückwärtsfahrzeit abgelaufen*/
BOOL _P2;
/* maximale Drehversuche abgelaufen*/
BOOL _P3;
/* Kurs im Sollbereich*/
BOOL _P4;
/* Störwartezeit abgelaufen*/
BOOL _P5;
DINT Fahrzeit;
/* Fahrzeit zu lange*/
BOOL _P6;
INT Drehversuche_merk;
};

VOID Kurs_fahren(struct Kurs_fahren *inst)
{
/* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/

/* VAR_TEMP */
BOOL _OV2;
BOOL _OV13;
BOOL _OV14;
BOOL _OV5;
BOOL _OV3;
BOOL _OV0;
BOOL _OV1;
BOOL _OV4;
BOOL _OV12;

/* Titel: Kurs_fahren */
/* Author: TeCAD */

/* Initialisierung der Operationsvariablen */
if (!init) {
  (*inst)._Op0 = 19;
}
/* Berechnung der nicht binären Prozeßvariablen */
(*inst)._P0 = kurs_diff > 10;
(*inst)._P1 = kurs_diff < -10;
(*inst)._P2 = (time_ms - (*inst).Zeit) > 2000;
(*inst)._P3 = (drehversuche - (*inst).Drehversuche_merk) > 3;
(*inst)._P4 = ABS(kurs_diff) < 150;
(*inst)._P5 = (time_ms - (*inst).Zeit) > 60000;
(*inst)._P6 = (time_ms - (*inst).Fahrzeit) > 120000;
/* Ausgabe der Operationsübergänge */
/* Auswahl der Operation 0 */
if ((*inst)._Op0 == 0) {
/* Übergang von Operation 0 zu Operation 2 */
  if (!pumpe_ausgeklappt) {
    (*inst)._Op0 = 2;
  }
/* Übergang von Operation 0 zu Operation 1 */
  if (pumpe_ausgeklappt) {
    (*inst)._Op0 = 1;
  }
}
/* Auswahl der Operation 1 */
else if ((*inst)._Op0 == 1) {
/* Übergang von Operation 1 zu Operation 15 */
  if (((*inst)._P2 && pumpe_ausgeklappt)) {
    (*inst)._Op0 = 15;
  }
/* Übergang von Operation 1 zu Operation 2 */
  if (((*inst)._P2 && !pumpe_ausgeklappt)) {
    (*inst)._Op0 = 2;
  }
}
/* Auswahl der Operation 2 */
else if ((*inst)._Op0 == 2) {
/* Übergang von Operation 2 zu Operation 12 */
  if (!antriebsfreigabe) {
    (*inst)._Op0 = 12;
  }
/* Übergang von Operation 2 zu Operation 0 */
  if ((antriebsfreigabe && pumpe_ausgeklappt)) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 2 zu Operation 13 */
  if ((antriebsfreigabe && !pumpe_ausgeklappt && (*inst)._P6)) {
    (*inst)._Op0 = 13;
  }
/* Übergang von Operation 2 zu Operation 5 */
  if ((antriebsfreigabe && !pumpe_ausgeklappt && !(*inst)._P6 && (*inst)._P4)) {
    (*inst)._Op0 = 5;
  }
/* Übergang von Operation 2 zu Operation 3 */
  if ((antriebsfreigabe && !pumpe_ausgeklappt && !(*inst)._P6 && !(*inst)._P4 && (*inst)._P3)) {
    (*inst)._Op0 = 3;
  }
}
/* Auswahl der Operation 3 */
else if ((*inst)._Op0 == 3) {
/* Übergang von Operation 3 zu Operation 4 */
  if (TRUE) {
    (*inst)._Op0 = 4;
  }
}
/* Auswahl der Operation 4 */
else if ((*inst)._Op0 == 4) {
/* Übergang von Operation 4 zu Operation 12 */
  if (!antriebsfreigabe) {
    (*inst)._Op0 = 12;
  }
/* Übergang von Operation 4 zu Operation 13 */
  if ((antriebsfreigabe && (*inst)._P6)) {
    (*inst)._Op0 = 13;
  }
/* Übergang von Operation 4 zu Operation 5 */
  if ((antriebsfreigabe && !(*inst)._P6 && (*inst)._P4)) {
    (*inst)._Op0 = 5;
  }
/* Übergang von Operation 4 zu Operation 0 */
  if ((antriebsfreigabe && !(*inst)._P6 && !(*inst)._P4 && (*inst)._P3)) {
    (*inst)._Op0 = 0;
  }
}
/* Auswahl der Operation 5 */
else if ((*inst)._Op0 == 5) {
/* Übergang von Operation 5 zu Operation 12 */
  if (!antriebsfreigabe) {
    (*inst)._Op0 = 12;
  }
/* Übergang von Operation 5 zu Operation 13 */
  if ((antriebsfreigabe && (*inst)._P6)) {
    (*inst)._Op0 = 13;
  }
/* Übergang von Operation 5 zu Operation 0 */
  if ((antriebsfreigabe && !(*inst)._P6 && !(*inst)._P4)) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 5 zu Operation 6 */
  if ((antriebsfreigabe && !(*inst)._P6 && (*inst)._P4 && pumpe_ausgeklappt)) {
    (*inst)._Op0 = 6;
  }
}
/* Auswahl der Operation 6 */
else if ((*inst)._Op0 == 6) {
/* Übergang von Operation 6 zu Operation 8 */
  if (!pumpe_ausgeklappt) {
    (*inst)._Op0 = 8;
  }
/* Übergang von Operation 6 zu Operation 7 */
  if (pumpe_ausgeklappt) {
    (*inst)._Op0 = 7;
  }
}
/* Auswahl der Operation 7 */
else if ((*inst)._Op0 == 7) {
/* Übergang von Operation 7 zu Operation 17 */
  if (((*inst)._P2 && pumpe_ausgeklappt)) {
    (*inst)._Op0 = 17;
  }
/* Übergang von Operation 7 zu Operation 8 */
  if (((*inst)._P2 && !pumpe_ausgeklappt)) {
    (*inst)._Op0 = 8;
  }
}
/* Auswahl der Operation 8 */
else if ((*inst)._Op0 == 8) {
/* Übergang von Operation 8 zu Operation 12 */
  if (!antriebsfreigabe) {
    (*inst)._Op0 = 12;
  }
/* Übergang von Operation 8 zu Operation 6 */
  if ((antriebsfreigabe && pumpe_ausgeklappt)) {
    (*inst)._Op0 = 6;
  }
/* Übergang von Operation 8 zu Operation 14 */
  if ((antriebsfreigabe && !pumpe_ausgeklappt && (*inst)._P6)) {
    (*inst)._Op0 = 14;
  }
/* Übergang von Operation 8 zu Operation 11 */
  if ((antriebsfreigabe && !pumpe_ausgeklappt && !(*inst)._P6 && (*inst)._P4)) {
    (*inst)._Op0 = 11;
  }
/* Übergang von Operation 8 zu Operation 9 */
  if ((antriebsfreigabe && !pumpe_ausgeklappt && !(*inst)._P6 && !(*inst)._P4 && (*inst)._P3)) {
    (*inst)._Op0 = 9;
  }
}
/* Auswahl der Operation 9 */
else if ((*inst)._Op0 == 9) {
/* Übergang von Operation 9 zu Operation 10 */
  if (TRUE) {
    (*inst)._Op0 = 10;
  }
}
/* Auswahl der Operation 10 */
else if ((*inst)._Op0 == 10) {
/* Übergang von Operation 10 zu Operation 12 */
  if (!antriebsfreigabe) {
    (*inst)._Op0 = 12;
  }
/* Übergang von Operation 10 zu Operation 14 */
  if ((antriebsfreigabe && (*inst)._P6)) {
    (*inst)._Op0 = 14;
  }
/* Übergang von Operation 10 zu Operation 11 */
  if ((antriebsfreigabe && !(*inst)._P6 && (*inst)._P4)) {
    (*inst)._Op0 = 11;
  }
/* Übergang von Operation 10 zu Operation 6 */
  if ((antriebsfreigabe && !(*inst)._P6 && !(*inst)._P4 && (*inst)._P3)) {
    (*inst)._Op0 = 6;
  }
}
/* Auswahl der Operation 11 */
else if ((*inst)._Op0 == 11) {
/* Übergang von Operation 11 zu Operation 12 */
  if (!antriebsfreigabe) {
    (*inst)._Op0 = 12;
  }
/* Übergang von Operation 11 zu Operation 14 */
  if ((antriebsfreigabe && (*inst)._P6)) {
    (*inst)._Op0 = 14;
  }
/* Übergang von Operation 11 zu Operation 6 */
  if ((antriebsfreigabe && !(*inst)._P6 && !(*inst)._P4)) {
    (*inst)._Op0 = 6;
  }
/* Übergang von Operation 11 zu Operation 0 */
  if ((antriebsfreigabe && !(*inst)._P6 && (*inst)._P4 && pumpe_ausgeklappt)) {
    (*inst)._Op0 = 0;
  }
}
/* Auswahl der Operation 12 */
else if ((*inst)._Op0 == 12) {
/* Übergang von Operation 12 zu Operation 5 */
  if (antriebsfreigabe) {
    (*inst)._Op0 = 5;
  }
}
/* Auswahl der Operation 13 */
else if ((*inst)._Op0 == 13) {
/* Übergang von Operation 13 zu Operation 6 */
  if (TRUE) {
    (*inst)._Op0 = 6;
  }
}
/* Auswahl der Operation 14 */
else if ((*inst)._Op0 == 14) {
/* Übergang von Operation 14 zu Operation 0 */
  if (TRUE) {
    (*inst)._Op0 = 0;
  }
}
/* Auswahl der Operation 15 */
else if ((*inst)._Op0 == 15) {
/* Übergang von Operation 15 zu Operation 16 */
  if (TRUE) {
    (*inst)._Op0 = 16;
  }
}
/* Auswahl der Operation 16 */
else if ((*inst)._Op0 == 16) {
/* Übergang von Operation 16 zu Operation 0 */
  if ((*inst)._P2) {
    (*inst)._Op0 = 0;
  }
}
/* Auswahl der Operation 17 */
else if ((*inst)._Op0 == 17) {
/* Übergang von Operation 17 zu Operation 18 */
  if (TRUE) {
    (*inst)._Op0 = 18;
  }
}
/* Auswahl der Operation 18 */
else if ((*inst)._Op0 == 18) {
/* Übergang von Operation 18 zu Operation 6 */
  if ((*inst)._P2) {
    (*inst)._Op0 = 6;
  }
}
else {
/* Initialisierung wenn keine Operation gesetzt ist */
  (*inst)._Op0 = 12;
}
/* Zuweisung der Operationsvariablen */
/* Kurs nach vorn */
_OV4 = ((*inst)._Op0 == 8) || ((*inst)._Op0 == 11);
/* Geschwindigkeit = vorwärts */
_OV1 = ((*inst)._Op0 == 5) || ((*inst)._Op0 == 11);
/* Kurs nach hinten */
_OV0 = ((*inst)._Op0 == 2) || ((*inst)._Op0 == 5);
/* Fahrzeit merken */
_OV12 = ((*inst)._Op0 == 12) || ((*inst)._Op0 == 13) || ((*inst)._Op0 == 14);
/* Geschwindigkeit = rückwärts */
_OV5 = ((*inst)._Op0 == 1) || ((*inst)._Op0 == 4) || ((*inst)._Op0 == 7) || ((*inst)._Op0 == 10);
/* Drehversuche merken */
_OV14 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 3) || ((*inst)._Op0 == 6) || ((*inst)._Op0 == 9);
/* Geschwindigkeit=0 */
_OV3 = ((*inst)._Op0 == 2) || ((*inst)._Op0 == 8) || ((*inst)._Op0 == 12) || ((*inst)._Op0 == 15) || ((*inst)._Op0 == 17);
/* Kurs = Kompass */
_OV13 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 6) || ((*inst)._Op0 == 12) || ((*inst)._Op0 == 15) || ((*inst)._Op0 == 17);
/* Zeit merken */
_OV2 = ((*inst)._Op0 == 5) || ((*inst)._Op0 == 11) || _OV13;
/* Ausgabe Sonderfunktionen und Timeraufrufe */
if (_OV0) {
/* Kurs nach hinten */
  kurs_akt = 2800;
}
if (_OV1) {
/* Geschwindigkeit = vorwärts */
  geschwindigkeit = 100;
}
if (_OV2) {
/* Zeit merken */
  (*inst).Zeit = time_ms;
}
if (_OV3) {
/* Geschwindigkeit=0 */
  geschwindigkeit = 0;
}
if (_OV4) {
/* Kurs nach vorn */
  kurs_akt = 1300;
}
if (_OV5) {
/* Geschwindigkeit = rückwärts */
  geschwindigkeit = -100;
}
if (((*inst)._Op0 == 12)) {
/* Statustext AUS */
  statustextnr = 1;
}
if (((*inst)._Op0 == 0)) {
/* Statustext DRHINT */
  statustextnr = 2;
}
if (((*inst)._Op0 == 5)) {
/* Statustext FAHINT */
  statustextnr = 3;
}
if (((*inst)._Op0 == 6)) {
/* Statustext DRVORN */
  statustextnr = 4;
}
if (((*inst)._Op0 == 11)) {
/* Statustext FAVORN */
  statustextnr = 5;
}
if (_OV12) {
/* Fahrzeit merken */
  (*inst).Fahrzeit = time_ms;
}
if (_OV13) {
/* Kurs = Kompass */
  kurs_akt = kompass_grad;
}
if (_OV14) {
/* Drehversuche merken */
  (*inst).Drehversuche_merk = drehversuche;
}
}
