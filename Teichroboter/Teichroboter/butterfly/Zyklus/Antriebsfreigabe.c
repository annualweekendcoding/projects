
struct Antriebsfreigabe {
/* VAR */
/* Operation 0 aktiv*/
BYTE _Op0;
};

VOID Antriebsfreigabe(struct Antriebsfreigabe *inst)
{
/* Automatisch generiert von TeCAD, bitte �nderungen nur dort vornehmen.*/

/* Titel: Antriebsfreigabe */
/* Author: TeCAD */

/* Initialisierung der Operationsvariablen */
if (!init) {
  (*inst)._Op0 = 2;
}
/* Ausgabe der Operations�berg�nge */
/* Auswahl der Operation 0 */
if ((*inst)._Op0 == 0) {
/* �bergang von Operation 0 zu Operation 1 */
  if ((!anlage_aus && !akku_niedrig)) {
    (*inst)._Op0 = 1;
  }
}
/* Auswahl der Operation 1 */
else if ((*inst)._Op0 == 1) {
/* �bergang von Operation 1 zu Operation 0 */
  if ((anlage_aus || akku_niedrig)) {
    (*inst)._Op0 = 0;
  }
}
else {
/* Initialisierung wenn keine Operation gesetzt ist */
  (*inst)._Op0 = 0;
}
/* Zuweisung der Operationsvariablen */
/* Antriebsfreigabe erteilt */
antriebsfreigabe = ((*inst)._Op0 == 1);
}
