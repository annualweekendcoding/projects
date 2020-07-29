// BEGIN_BLOCK Pumpensteuerung
void Pumpensteuerung ()
{
  /* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/
  /* VAR_TEMP */
  uint8_t _OV10 = false;
  uint8_t _OV7 = false;
  uint8_t _OV6 = false;
  /* Titel: Pumpensteuerung */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    F(uint8_t,28) = ((uint8_t) (10));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht binären Prozeßvariablen */
  FX(34,2) = F(int16_t,18) >= R(int16_t,0);
  FX(34,3) = F(int16_t,10) > 327;
  FX(34,4) = F(int16_t,10) <= 0 || F(int16_t,12) <= 0;
  FX(34,5) = (I(int16_t,14) + 2) > I(int16_t,8);
  FX(34,6) = F(int16_t,18) >= R(int16_t,2);
  FX(34,7) = I(int16_t,14) <= ((int16_t) (-50));
  FX(35,0) = F(int16_t,18) >= R(int16_t,4);
  FX(35,1) = F(int16_t,10) > 3270;
  FX(35,2) = F(int16_t,18) >= R(int16_t,6);
  FX(35,3) = F(int16_t,18) >= 180;
  FX(35,4) = I(int16_t,8) > R(int16_t,8);
  FX(35,5) = (I(int16_t,8) + 2) > I(int16_t,14);
  FX(35,6) = I(int16_t,8) < R(int16_t,10);
  FX(35,7) = I(int16_t,8) <= ((int16_t) (-50));
  if (F(uint8_t,28) == ((uint8_t) (0)))
  {
    if ((!FX(0,3) && FX(34,2) && IX(0,0)))
    {
      F(uint8_t,28) = ((uint8_t) (7));
    }
    if ((!FX(0,3) && FX(34,2) && !IX(0,0) && IX(0,1) && FX(34,6)))
    {
      F(uint8_t,28) = ((uint8_t) (8));
    }
    if ((!FX(0,3) && FX(34,2) && !IX(0,0) && ((!IX(0,1) && (FX(34,7) || FX(35,7) || (FX(35,4) && FX(35,5))
     || (!FX(35,4) && (!FX(35,6) || FX(34,5))))) || (((IX(0,1) && !FX(34,6)) || (!IX(0,1) && !FX(34,7) &&
     !FX(35,7) && ((FX(35,4) && !FX(35,5)) || (!FX(35,4) && FX(35,6) && !FX(34,5))))) && FX(35,0)))))
    {
      F(uint8_t,28) = ((uint8_t) (3));
    }
  }
  else if (F(uint8_t,28) == ((uint8_t) (1)))
  {
    if (FX(0,3))
    {
      F(uint8_t,28) = ((uint8_t) (4));
    }
    if ((!FX(0,3) && FX(34,2) && (FX(35,2) || IX(0,0) || (!FX(34,7) && !FX(35,7) && ((FX(35,4) && !FX(35,5))
     || (!FX(35,4) && FX(35,6) && !FX(34,5))) && FX(35,3)))))
    {
      F(uint8_t,28) = ((uint8_t) (5));
    }
  }
  else if (F(uint8_t,28) == ((uint8_t) (2)))
  {
    F(uint8_t,28) = ((uint8_t) (0));
  }
  else if (F(uint8_t,28) == ((uint8_t) (3)))
  {
    F(uint8_t,28) = ((uint8_t) (1));
  }
  else if (F(uint8_t,28) == ((uint8_t) (4)))
  {
    F(uint8_t,28) = ((uint8_t) (0));
  }
  else if (F(uint8_t,28) == ((uint8_t) (5)))
  {
    if (FX(34,4))
    {
      F(uint8_t,28) = ((uint8_t) (4));
    }
    if ((!FX(34,4) && !FX(34,3)))
    {
      F(uint8_t,28) = ((uint8_t) (2));
    }
    if ((!FX(34,4) && FX(34,3) && !FX(35,1)))
    {
      F(uint8_t,28) = ((uint8_t) (6));
    }
    if ((!FX(34,4) && FX(34,3) && FX(35,1)))
    {
      F(uint8_t,28) = ((uint8_t) (9));
    }
  }
  else if (F(uint8_t,28) == ((uint8_t) (6)))
  {
    F(uint8_t,28) = ((uint8_t) (0));
  }
  else if (F(uint8_t,28) == ((uint8_t) (7)))
  {
    if (!IX(0,0))
    {
      F(uint8_t,28) = ((uint8_t) (4));
    }
  }
  else if (F(uint8_t,28) == ((uint8_t) (8)))
  {
    if (!IX(0,1))
    {
      F(uint8_t,28) = ((uint8_t) (4));
    }
  }
  else if (F(uint8_t,28) == ((uint8_t) (9)))
  {
    F(uint8_t,28) = ((uint8_t) (0));
  }
  else
  {
    F(uint8_t,28) = ((uint8_t) (4));
  }
  /* Ausgabe der Operationsübergänge */
  /* Auswahl der Operation 0 */
  /* Übergang von Operation 0 zu Operation 7 */
  /* Übergang von Operation 0 zu Operation 8 */
  /* Übergang von Operation 0 zu Operation 3 */
  /* Auswahl der Operation 1 */
  /* Übergang von Operation 1 zu Operation 4 */
  /* Übergang von Operation 1 zu Operation 5 */
  /* Auswahl der Operation 2 */
  /* Übergang von Operation 2 zu Operation 0 */
  /* Auswahl der Operation 3 */
  /* Übergang von Operation 3 zu Operation 1 */
  /* Auswahl der Operation 4 */
  /* Übergang von Operation 4 zu Operation 0 */
  /* Auswahl der Operation 5 */
  /* Übergang von Operation 5 zu Operation 4 */
  /* Übergang von Operation 5 zu Operation 2 */
  /* Übergang von Operation 5 zu Operation 6 */
  /* Übergang von Operation 5 zu Operation 9 */
  /* Auswahl der Operation 6 */
  /* Übergang von Operation 6 zu Operation 0 */
  /* Auswahl der Operation 7 */
  /* Übergang von Operation 7 zu Operation 4 */
  /* Auswahl der Operation 8 */
  /* Übergang von Operation 8 zu Operation 4 */
  /* Auswahl der Operation 9 */
  /* Übergang von Operation 9 zu Operation 0 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Pumpe einschalten */
  QX(0,0) = (F(uint8_t,28) == ((uint8_t) (1)));
  /* Störung Sensor Pumpenschacht */
  FX(0,1) = (F(uint8_t,28) == ((uint8_t) (7)));
  /* Störung Sensor Filtergraben */
  FX(0,2) = (F(uint8_t,28) == ((uint8_t) (8)));
  /* Zeitdifferenz berechnen */
  _OV10 = (F(uint8_t,28) == ((uint8_t) (0))) || (F(uint8_t,28) == ((uint8_t) (1)));
  /* Zeitverhältnis mitteln */
  _OV6 = (F(uint8_t,28) == ((uint8_t) (2))) || (F(uint8_t,28) == ((uint8_t) (6))) || (F(uint8_t,28) == ((uint8_t) (9)));
  /* Zeit merken */
  _OV7 = (F(uint8_t,28) == ((uint8_t) (3))) || (F(uint8_t,28) == ((uint8_t) (4))) || _OV6;
  if ((F(uint8_t,28) == ((uint8_t) (5))))
  {
    F(int16_t,10) = F(int16_t,4) - F(int16_t,30);
  }
  if ((F(uint8_t,28) == ((uint8_t) (3))))
  {
    F(int16_t,12) = F(int16_t,4) - F(int16_t,30);
  }
  if ((F(uint8_t,28) == ((uint8_t) (2))))
  {
    F(int16_t,14) = ((int16_t) ((F(int16_t,10) * 100) / (F(int16_t,10) + F(int16_t,12))));
  }
  if ((F(uint8_t,28) == ((uint8_t) (6))))
  {
    F(int16_t,14) = ((int16_t) ((F(int16_t,10) * 10) / (F(int16_t,10) / 10 + F(int16_t,12) / 10)));
  }
  if ((F(uint8_t,28) == ((uint8_t) (9))))
  {
    F(int16_t,14) = ((int16_t) (F(int16_t,10) / (F(int16_t,10) / 100 + F(int16_t,12) / 100)));
  }
  if (_OV6)
  {
    F(int16_t,16) = ((int16_t) ((F(int16_t,16) * 4) / 5 + F(int16_t,14) / 5));
  }
  if (_OV7)
  {
    F(int16_t,30) = F(int16_t,4);
  }
  if (_OV10)
  {
    F(int16_t,18) = F(int16_t,4) - F(int16_t,30);
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Einschaltzeit merken */
  /* Ausschaltzeit merken */
  /* Zeitverhältnis berechnen */
  /* Zeitverhaeltnis mit Zenteln berechnen */
  /* Zeitverhaeltnis mit /100 berechnen */
  /* Zeitverhältnis mitteln */
  /* Zeit merken */
  /* Zeitdifferenz berechnen */
}
// END_BLOCK

