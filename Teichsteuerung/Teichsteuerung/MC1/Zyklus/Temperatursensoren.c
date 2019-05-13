// BEGIN_BLOCK Temperatursensoren
struct Temperatursensoren {
  /* VAR */
  /* Operation 0 aktiv*/
  uint8_t Temperatursensoren_Op0;
  /* 10s vergangen*/
  bool Temperatursensoren_P0;
  /* weitere 10ms vergangen*/
  bool Temperatursensoren_P1;
  int16_t diff;
};
void Temperatursensoren (
  struct Temperatursensoren *inst
  )
{
  /* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/
  /* VAR_TEMP */
  bool _OV3;
  /* Titel: Temperatursensoren */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    (*inst).Temperatursensoren_Op0 = ((uint8_t) (4));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht binären Prozeßvariablen */
  (*inst).Temperatursensoren_P0 = (*inst).diff > 10000;
  (*inst).Temperatursensoren_P1 = (*inst).diff > 10010;
  if ((*inst).Temperatursensoren_Op0 == ((uint8_t) (0)))
  {
    (*inst).Temperatursensoren_Op0 = ((uint8_t) (1));
  }
  else if ((*inst).Temperatursensoren_Op0 == ((uint8_t) (1)))
  {
    if ((*inst).Temperatursensoren_P0)
    {
      (*inst).Temperatursensoren_Op0 = ((uint8_t) (3));
    }
  }
  else if ((*inst).Temperatursensoren_Op0 == ((uint8_t) (2)))
  {
    (*inst).Temperatursensoren_Op0 = ((uint8_t) (0));
  }
  else if ((*inst).Temperatursensoren_Op0 == ((uint8_t) (3)))
  {
    if ((*inst).Temperatursensoren_P1)
    {
      (*inst).Temperatursensoren_Op0 = ((uint8_t) (2));
    }
  }
  else
  {
    (*inst).Temperatursensoren_Op0 = ((uint8_t) (0));
  }
  /* Ausgabe der Operationsübergänge */
  /* Auswahl der Operation 0 */
  /* Übergang von Operation 0 zu Operation 1 */
  /* Auswahl der Operation 1 */
  /* Übergang von Operation 1 zu Operation 3 */
  /* Auswahl der Operation 2 */
  /* Übergang von Operation 2 zu Operation 0 */
  /* Auswahl der Operation 3 */
  /* Übergang von Operation 3 zu Operation 2 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Temperaturen vom OneWire einlesen */
  QX(0,1) = ((*inst).Temperatursensoren_Op0 == ((uint8_t) (2)));
  /* Spannung für Temperatursensoren einschalten */
  QX(0,6) = ((*inst).Temperatursensoren_Op0 == ((uint8_t) (2))) || ((*inst).Temperatursensoren_Op0 == ((uint8_t) (3)));
  /* Differenz berechnen */
  _OV3 = ((*inst).Temperatursensoren_Op0 == ((uint8_t) (1))) || ((*inst).Temperatursensoren_Op0 == ((uint8_t) (3)));
  if (((*inst).Temperatursensoren_Op0 == ((uint8_t) (0))))
  {
    F(int16_t,8) = F(int16_t,2);
  }
  if (_OV3)
  {
    (*inst).diff = F(int16_t,2) - F(int16_t,8);
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Zeit merken */
  /* Differenz berechnen */
}
// END_BLOCK

