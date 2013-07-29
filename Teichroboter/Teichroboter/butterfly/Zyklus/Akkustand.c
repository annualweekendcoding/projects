
struct Akkustand {
/* VAR */
/* Operation 0 aktiv*/
BYTE _Op0;
/* Akku unter Minimum*/
BOOL _P0;
/* Akku �ber Wiedereinschalten*/
BOOL _P1;
};

VOID Akkustand(struct Akkustand *inst)
{
/* Automatisch generiert von TeCAD, bitte �nderungen nur dort vornehmen.*/

/* Titel: Akkustand */
/* Author: TeCAD */

/* Initialisierung der Operationsvariablen */
if (!init) {
  (*inst)._Op0 = 2;
}
/* Berechnung der nicht bin�ren Proze�variablen */
(*inst)._P0 = akku_spannung < 1000;
(*inst)._P1 = akku_spannung > 1200;
/* Ausgabe der Operations�berg�nge */
/* Auswahl der Operation 0 */
if ((*inst)._Op0 == 0) {
/* �bergang von Operation 0 zu Operation 1 */
  if ((*inst)._P0) {
    (*inst)._Op0 = 1;
  }
}
/* Auswahl der Operation 1 */
else if ((*inst)._Op0 == 1) {
/* �bergang von Operation 1 zu Operation 0 */
  if (((*inst)._P1 && !(*inst)._P0)) {
    (*inst)._Op0 = 0;
  }
}
else {
/* Initialisierung wenn keine Operation gesetzt ist */
  (*inst)._Op0 = 0;
}
/* Zuweisung der Operationsvariablen */
/* Akku niedrig */
akku_niedrig = ((*inst)._Op0 == 1);
}
