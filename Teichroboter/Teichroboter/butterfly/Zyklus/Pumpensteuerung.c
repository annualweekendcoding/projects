
struct Pumpensteuerung {
/* VAR */
/* Operation 0 aktiv*/
BYTE _Op0;
/* Akkuspannung zu hoch*/
BOOL _P0;
DINT Zeit;
/* Pumpzeit vergangen*/
BOOL _P1;
/* Wartezeit vergangen*/
BOOL _P2;
/* Kompass im Pumpbereich*/
BOOL _P3;
BOOL _P4;
};

VOID Pumpensteuerung(struct Pumpensteuerung *inst)
{
/* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/

/* VAR_TEMP */
BOOL _OV0;

/* Titel: Pumpensteuerung */
/* Author: TeCAD */

/* Initialisierung der Operationsvariablen */
if (!init) {
  (*inst)._Op0 = 4;
}
/* Berechnung der nicht binären Prozeßvariablen */
(*inst)._P0 = akku_spannung > 1420;
(*inst)._P1 = (time_ms - (*inst).Zeit) > 3000;
(*inst)._P2 = (time_ms - (*inst).Zeit) > 20000;
(*inst)._P3 = kompass_grad < 1800 && kompass_grad > 700;
/* Ausgabe der Operationsübergänge */
/* Auswahl der Operation 0 */
if ((*inst)._Op0 == 0) {
/* Übergang von Operation 0 zu Operation 1 */
  if (((*inst)._P0 || (!akku_niedrig && !anlage_aus && !pumpe_ausgeklappt && (*inst)._P3))) {
    (*inst)._Op0 = 1;
  }
}
/* Auswahl der Operation 1 */
else if ((*inst)._Op0 == 1) {
/* Übergang von Operation 1 zu Operation 0 */
  if ((akku_niedrig || anlage_aus)) {
    (*inst)._Op0 = 0;
  }
/* Übergang von Operation 1 zu Operation 2 */
  if ((!akku_niedrig && !anlage_aus && (*inst)._P1)) {
    (*inst)._Op0 = 2;
  }
}
/* Auswahl der Operation 2 */
else if ((*inst)._Op0 == 2) {
/* Übergang von Operation 2 zu Operation 3 */
  if (TRUE) {
    (*inst)._Op0 = 3;
  }
}
/* Auswahl der Operation 3 */
else if ((*inst)._Op0 == 3) {
/* Übergang von Operation 3 zu Operation 0 */
  if ((*inst)._P2) {
    (*inst)._Op0 = 0;
  }
}
else {
/* Initialisierung wenn keine Operation gesetzt ist */
  (*inst)._Op0 = 0;
}
/* Zuweisung der Operationsvariablen */
/* Pumpe einschalten */
pumpe_ein = ((*inst)._Op0 == 1);
/* Zeit merken */
_OV0 = ((*inst)._Op0 == 0) || ((*inst)._Op0 == 2);
/* Ausgabe Sonderfunktionen und Timeraufrufe */
if (_OV0) {
/* Zeit merken */
  (*inst).Zeit = time_ms;
}
}
