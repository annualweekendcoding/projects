// BEGIN_BLOCK Solaranlage
void Solaranlage ()
{
  /* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/
  /* Titel: Solaranlage */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    F(uint8_t,29) = ((uint8_t) (2));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht binären Prozeßvariablen */
  FX(36,0) = I(int16_t,12) > 40;
  FX(36,1) = I(int16_t,10) > I(int16_t,8);
  FX(36,3) = I(int16_t,8) > R(int16_t,8);
  FX(36,4) = I(int16_t,8) <= ((int16_t) (-50));
  FX(36,2) = I(int16_t,10) <= ((int16_t) (-50));
  FX(36,5) = I(int16_t,12) <= ((int16_t) (-50));
  if (F(uint8_t,29) == ((uint8_t) (0)))
  {
    if ((!FX(36,2) && !FX(36,4) && (FX(36,0) || FX(36,5)) && ((FX(36,3) && !FX(36,1)) || (!FX(36,3) && FX(36,1)))))
    {
      F(uint8_t,29) = ((uint8_t) (1));
    }
  }
  else if (F(uint8_t,29) == ((uint8_t) (1)))
  {
    if ((FX(36,2) || FX(36,4) || (!FX(36,0) && !FX(36,5)) || (FX(36,3) && FX(36,1)) || (!FX(36,3) && !FX(36,1))))
    {
      F(uint8_t,29) = ((uint8_t) (0));
    }
  }
  else
  {
    F(uint8_t,29) = ((uint8_t) (0));
  }
  /* Ausgabe der Operationsübergänge */
  /* Auswahl der Operation 0 */
  /* Übergang von Operation 0 zu Operation 1 */
  /* Auswahl der Operation 1 */
  /* Übergang von Operation 1 zu Operation 0 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Magnetventil Fischwasser einschalten */
  QX(0,2) = (F(uint8_t,29) == ((uint8_t) (1)));
  if ((F(uint8_t,29) == ((uint8_t) (0))))
  {
    F(int16_t,32) = F(int16_t,4);
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Zeit merken */
}
// END_BLOCK

