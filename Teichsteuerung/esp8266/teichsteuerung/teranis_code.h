#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_akttimes_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_akttimes_st

// BEGIN_BLOCK akttimes
void akttimes ()
{
  /* VAR_TEMP */
  int64_t l = 0;
  int16_t i = 0;
  l = IREF(int64_t,20) - FREF(int64_t,60);
  if (l > 0 && l < 10000)
  {
    FINT(2) = ((int16_t) (FINT(2) + l));
  }
  /* Millisekunden Ticker */
  FREF(int64_t,60) = IREF(int64_t,20);
  i = FINT(2) - FINT(6);
  if (i < 0)
  {
    FINT(6) = FINT(2);
  }
  else if (i > 1000)
  {
    FINT(4) = ((int16_t) (FINT(4) + i / 1000));
    FINT(6) = ((int16_t) (FINT(6) + (i / 1000) * 1000));
  }
}
// END_BLOCK

#endif

#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Pumpensteuerung_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Pumpensteuerung_st

// BEGIN_BLOCK Pumpensteuerung
struct Pumpensteuerung {
  /* VAR */
  /* aktive Operationsnummer*/
  uint8_t Pumpensteuerung_Op0;
  /* gemerkte Zeit*/
  int16_t time_last;
  /* Mindestenstzeit vergangen*/
  bool Pumpensteuerung_P0;
  /* Einschaltzeit/10?*/
  bool Pumpensteuerung_P1;
  /* Zeit�berlauf*/
  bool Pumpensteuerung_P2;
  /* Temperatur an Pumpe gr��er als im Teich*/
  bool Pumpensteuerung_P3;
  /* maximale Nachf�llzeit vergangen*/
  bool Pumpensteuerung_P4;
  /* Temperatursensor gest�rt*/
  bool Pumpensteuerung_P5;
  /* maximale Ausschaltzeit vergangen*/
  bool Pumpensteuerung_P6;
  /* Einschaltzeit/100 ?*/
  bool Pumpensteuerung_P7;
  /* maximale Einschaltzeit vergangen*/
  bool Pumpensteuerung_P8;
  /* maximale EIN-Zeit bei Fehltemperatur vergangen*/
  bool Pumpensteuerung_P9;
  /* Temperatur im Teich zu gross*/
  bool Pumpensteuerung_P10;
  /* Temperatur im Teich gr��er als an Pumpe*/
  bool Pumpensteuerung_P11;
  /* Temperatur im Teich zu klein*/
  bool Pumpensteuerung_P12;
};
void Pumpensteuerung (
  struct Pumpensteuerung *inst
  )
{
  /* Automatisch generiert von TeCAD, bitte �nderungen nur dort vornehmen.*/
  /* VAR_TEMP */
  bool _OV10;
  bool _OV7;
  bool _OV6;
  /* Titel: Pumpensteuerung */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (10));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht bin�ren Proze�variablen */
  (*inst).Pumpensteuerung_P0 = FINT(18) >= FINT(20);
  (*inst).Pumpensteuerung_P1 = FINT(10) > 327;
  (*inst).Pumpensteuerung_P2 = FINT(10) <= 0 || FINT(12) <= 0;
  (*inst).Pumpensteuerung_P3 = (IINT(14) + 2) > IINT(8);
  (*inst).Pumpensteuerung_P4 = FINT(18) >= FINT(22);
  (*inst).Pumpensteuerung_P5 = (IINT(14) < ((int16_t) (-50))) || (IINT(8) < ((int16_t) (-50)));
  (*inst).Pumpensteuerung_P6 = FINT(18) >= FINT(24);
  (*inst).Pumpensteuerung_P7 = FINT(10) > 3270;
  (*inst).Pumpensteuerung_P8 = FINT(18) >= FINT(26);
  (*inst).Pumpensteuerung_P9 = FINT(18) >= 180;
  (*inst).Pumpensteuerung_P10 = IINT(8) > FINT(28);
  (*inst).Pumpensteuerung_P11 = (IINT(8) + 2) > IINT(14);
  (*inst).Pumpensteuerung_P12 = IINT(8) < FINT(30);
  if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (0)))
  {
    if ((!FX(0,3) && (*inst).Pumpensteuerung_P0 && IX(0,0)))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (7));
    }
    if ((!FX(0,3) && (*inst).Pumpensteuerung_P0 && !IX(0,0) && IX(0,1) && (*inst).Pumpensteuerung_P4))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (8));
    }
    if ((!FX(0,3) && (*inst).Pumpensteuerung_P0 && !IX(0,0) && ((!IX(0,1) && ((*inst).Pumpensteuerung_P5
     || ((*inst).Pumpensteuerung_P10 && (*inst).Pumpensteuerung_P11) || (!(*inst).Pumpensteuerung_P10 &&
     (!(*inst).Pumpensteuerung_P12 || (*inst).Pumpensteuerung_P3)))) || (((IX(0,1) && !(*inst).Pumpensteuerung_P4)
     || (!IX(0,1) && !(*inst).Pumpensteuerung_P5 && (((*inst).Pumpensteuerung_P10 && !(*inst).Pumpensteuerung_P11)
     || (!(*inst).Pumpensteuerung_P10 && (*inst).Pumpensteuerung_P12 && !(*inst).Pumpensteuerung_P3)))) && (*inst).Pumpensteuerung_P6))))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (3));
    }
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (1)))
  {
    if (FX(0,3))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
    }
    if ((!FX(0,3) && (*inst).Pumpensteuerung_P0 && ((*inst).Pumpensteuerung_P8 || IX(0,0) || (!(*inst).Pumpensteuerung_P5
     && (((*inst).Pumpensteuerung_P10 && !(*inst).Pumpensteuerung_P11) || (!(*inst).Pumpensteuerung_P10 &&
     (*inst).Pumpensteuerung_P12 && !(*inst).Pumpensteuerung_P3)) && (*inst).Pumpensteuerung_P9))))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (5));
    }
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (2)))
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (3)))
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (1));
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (4)))
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (5)))
  {
    if ((*inst).Pumpensteuerung_P2)
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
    }
    if ((!(*inst).Pumpensteuerung_P2 && !(*inst).Pumpensteuerung_P1))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (2));
    }
    if ((!(*inst).Pumpensteuerung_P2 && (*inst).Pumpensteuerung_P1 && !(*inst).Pumpensteuerung_P7))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (6));
    }
    if ((!(*inst).Pumpensteuerung_P2 && (*inst).Pumpensteuerung_P1 && (*inst).Pumpensteuerung_P7))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (9));
    }
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (6)))
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (7)))
  {
    if (!IX(0,0))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
    }
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (8)))
  {
    if (!IX(0,1))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
    }
  }
  else if ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (9)))
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
  }
  /* Ausgabe der Operations�berg�nge */
  /* Auswahl der Operation 0 */
  /* �bergang von Operation 0 zu Operation 7 */
  /* �bergang von Operation 0 zu Operation 8 */
  /* �bergang von Operation 0 zu Operation 3 */
  /* Auswahl der Operation 1 */
  /* �bergang von Operation 1 zu Operation 4 */
  /* �bergang von Operation 1 zu Operation 5 */
  /* Auswahl der Operation 2 */
  /* �bergang von Operation 2 zu Operation 0 */
  /* Auswahl der Operation 3 */
  /* �bergang von Operation 3 zu Operation 1 */
  /* Auswahl der Operation 4 */
  /* �bergang von Operation 4 zu Operation 0 */
  /* Auswahl der Operation 5 */
  /* �bergang von Operation 5 zu Operation 4 */
  /* �bergang von Operation 5 zu Operation 2 */
  /* �bergang von Operation 5 zu Operation 6 */
  /* �bergang von Operation 5 zu Operation 9 */
  /* Auswahl der Operation 6 */
  /* �bergang von Operation 6 zu Operation 0 */
  /* Auswahl der Operation 7 */
  /* �bergang von Operation 7 zu Operation 4 */
  /* Auswahl der Operation 8 */
  /* �bergang von Operation 8 zu Operation 4 */
  /* Auswahl der Operation 9 */
  /* �bergang von Operation 9 zu Operation 0 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Pumpe einschalten */
  QX(0,0) = ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (1)));
  /* St�rung Sensor Pumpenschacht */
  FX(0,1) = ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (7)));
  /* St�rung Sensor Filtergraben */
  FX(0,2) = ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (8)));
  /* Zeitdifferenz berechnen */
  _OV10 = ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (0))) || ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (1)));
  /* Zeitverh�ltnis mitteln */
  _OV6 = ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (2))) || ((*inst).Pumpensteuerung_Op0 == ((uint8_t)
   (6))) || ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (9)));
  /* Zeit merken */
  _OV7 = ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (3))) || ((*inst).Pumpensteuerung_Op0 == ((uint8_t) (4))) || _OV6;
  if (((*inst).Pumpensteuerung_Op0 == ((uint8_t) (5))))
  {
    FINT(10) = FINT(4) - (*inst).time_last;
  }
  if (((*inst).Pumpensteuerung_Op0 == ((uint8_t) (3))))
  {
    FINT(12) = FINT(4) - (*inst).time_last;
  }
  if (((*inst).Pumpensteuerung_Op0 == ((uint8_t) (2))))
  {
    FINT(14) = ((int16_t) ((FINT(10) * 100) / (FINT(10) + FINT(12))));
  }
  if (((*inst).Pumpensteuerung_Op0 == ((uint8_t) (6))))
  {
    FINT(14) = ((int16_t) ((FINT(10) * 10) / (FINT(10) / 10 + FINT(12) / 10)));
  }
  if (((*inst).Pumpensteuerung_Op0 == ((uint8_t) (9))))
  {
    FINT(14) = ((int16_t) (FINT(10) / (FINT(10) / 100 + FINT(12) / 100)));
  }
  if (_OV6)
  {
    FINT(16) = ((int16_t) ((FINT(16) * 4) / 5 + FINT(14) / 5));
  }
  if (_OV7)
  {
    (*inst).time_last = FINT(4);
  }
  if (_OV10)
  {
    FINT(18) = FINT(4) - (*inst).time_last;
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Einschaltzeit merken */
  /* Ausschaltzeit merken */
  /* Zeitverh�ltnis berechnen */
  /* Zeitverhaeltnis mit Zenteln berechnen */
  /* Zeitverhaeltnis mit /100 berechnen */
  /* Zeitverh�ltnis mitteln */
  /* Zeit merken */
  /* Zeitdifferenz berechnen */
}
// END_BLOCK

#endif

#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Temperatursensoren_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Temperatursensoren_st

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
  /* Automatisch generiert von TeCAD, bitte �nderungen nur dort vornehmen.*/
  /* VAR_TEMP */
  bool _OV3;
  /* Titel: Temperatursensoren */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    (*inst).Temperatursensoren_Op0 = ((uint8_t) (4));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht bin�ren Proze�variablen */
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
  /* Ausgabe der Operations�berg�nge */
  /* Auswahl der Operation 0 */
  /* �bergang von Operation 0 zu Operation 1 */
  /* Auswahl der Operation 1 */
  /* �bergang von Operation 1 zu Operation 3 */
  /* Auswahl der Operation 2 */
  /* �bergang von Operation 2 zu Operation 0 */
  /* Auswahl der Operation 3 */
  /* �bergang von Operation 3 zu Operation 2 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Temperaturen vom OneWire einlesen */
  QX(0,1) = ((*inst).Temperatursensoren_Op0 == ((uint8_t) (2)));
  /* Spannung f�r Temperatursensoren einschalten */
  QX(0,6) = ((*inst).Temperatursensoren_Op0 == ((uint8_t) (2))) || ((*inst).Temperatursensoren_Op0 == ((uint8_t) (3)));
  /* Differenz berechnen */
  _OV3 = ((*inst).Temperatursensoren_Op0 == ((uint8_t) (1))) || ((*inst).Temperatursensoren_Op0 == ((uint8_t) (3)));
  if (((*inst).Temperatursensoren_Op0 == ((uint8_t) (0))))
  {
    FINT(8) = FINT(2);
  }
  if (_OV3)
  {
    (*inst).diff = FINT(2) - FINT(8);
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Zeit merken */
  /* Differenz berechnen */
}
// END_BLOCK

#endif

#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Zyklus_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Zyklus_st

// BEGIN_BLOCK Zyklus
struct Zyklus {
  /* VAR */
  struct Pumpensteuerung I_Pumpensteuerung;
  struct Temperatursensoren I_Temperatursensoren;
};
void Zyklus (
  struct Zyklus *inst
  )
{
  /* automatisch generiert von TERANiS-Studio, bitte �nderungen nur dort vornehmen.*/
  /* Titel: Zyklus */
  /* Author: TERANiS */
  akttimes();
  Pumpensteuerung(&((*inst).I_Pumpensteuerung));
  Temperatursensoren(&((*inst).I_Temperatursensoren));
  /* akttimes.st */
  /* Pumpensteuerung.prp */
  /* Temperatursensoren.prp */
  /* Initialisierungsvariable setzen */
  FX(0,0) = true;
}
// END_BLOCK

#endif

