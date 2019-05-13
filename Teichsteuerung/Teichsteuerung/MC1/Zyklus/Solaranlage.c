// BEGIN_BLOCK Solaranlage
struct Solaranlage {
  /* VAR */
  /* Operation 0 aktiv*/
  uint8_t Solaranlage_Op0;
  /* Aussentemperatur > 4�C*/
  bool Solaranlage_P0;
  /* Solartemperatur > Teichtemperatur*/
  bool Solaranlage_P1;
  int16_t z;
};
void Solaranlage (
  struct Solaranlage *inst
  )
{
  /* Automatisch generiert von TeCAD, bitte �nderungen nur dort vornehmen.*/
  /* Titel: Solaranlage */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    (*inst).Solaranlage_Op0 = ((uint8_t) (2));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht bin�ren Proze�variablen */
  (*inst).Solaranlage_P0 = I(int16_t,12) > 40;
  (*inst).Solaranlage_P1 = I(int16_t,10) > I(int16_t,8);
  if ((*inst).Solaranlage_Op0 == ((uint8_t) (0)))
  {
    if (((*inst).Solaranlage_P0 && (*inst).Solaranlage_P1))
    {
      (*inst).Solaranlage_Op0 = ((uint8_t) (1));
    }
  }
  else if ((*inst).Solaranlage_Op0 == ((uint8_t) (1)))
  {
    if ((!(*inst).Solaranlage_P0 || !(*inst).Solaranlage_P1))
    {
      (*inst).Solaranlage_Op0 = ((uint8_t) (0));
    }
  }
  else
  {
    (*inst).Solaranlage_Op0 = ((uint8_t) (0));
  }
  /* Ausgabe der Operations�berg�nge */
  /* Auswahl der Operation 0 */
  /* �bergang von Operation 0 zu Operation 1 */
  /* Auswahl der Operation 1 */
  /* �bergang von Operation 1 zu Operation 0 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Magnetventil Fischwasser einschalten */
  QX(0,2) = ((*inst).Solaranlage_Op0 == ((uint8_t) (1)));
  if (((*inst).Solaranlage_Op0 == ((uint8_t) (0))))
  {
    (*inst).z = F(int16_t,4);
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Zeit merken */
}
// END_BLOCK

