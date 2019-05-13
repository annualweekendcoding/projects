#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_akttimes_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_akttimes_st

// BEGIN_BLOCK akttimes
void akttimes ()
{
  /* VAR_TEMP */
  int64_t l = 0;
  int16_t i = 0;
  l = I(int64_t,20) - F(int64_t,60);
  if (l > 0 && l < 10000)
  {
    F(int16_t,2) = ((int16_t) (F(int16_t,2) + l));
  }
  /* Millisekunden Ticker */
  F(int64_t,60) = I(int64_t,20);
  i = F(int16_t,2) - F(int16_t,6);
  if (i < 0)
  {
    F(int16_t,6) = F(int16_t,2);
  }
  else if (i > 1000)
  {
    F(int16_t,4) = ((int16_t) (F(int16_t,4) + i / 1000));
    F(int16_t,6) = ((int16_t) (F(int16_t,6) + (i / 1000) * 1000));
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
  /* Mindestzeit vergangen*/
  bool Pumpensteuerung_P0;
  /* Einschaltzeit/10?*/
  bool Pumpensteuerung_P1;
  /* Zeitüberlauf*/
  bool Pumpensteuerung_P2;
  /* Temperatur an Pumpe größer als im Teich*/
  bool Pumpensteuerung_P3;
  /* maximale Nachfüllzeit vergangen*/
  bool Pumpensteuerung_P4;
  /* Temperatursensor gestört*/
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
  /* Temperatur im Teich größer als an Pumpe*/
  bool Pumpensteuerung_P11;
  /* Temperatur im Teich zu klein*/
  bool Pumpensteuerung_P12;
};
void Pumpensteuerung (
  struct Pumpensteuerung *inst
  )
{
  /* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/
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
  /* Berechnung der nicht binären Prozeßvariablen */
  (*inst).Pumpensteuerung_P0 = F(int16_t,18) >= R(int16_t,0);
  (*inst).Pumpensteuerung_P1 = F(int16_t,10) > 327;
  (*inst).Pumpensteuerung_P2 = F(int16_t,10) <= 0 || F(int16_t,12) <= 0;
  (*inst).Pumpensteuerung_P3 = (I(int16_t,14) + 2) > I(int16_t,8);
  (*inst).Pumpensteuerung_P4 = F(int16_t,18) >= R(int16_t,2);
  (*inst).Pumpensteuerung_P5 = (I(int16_t,14) < ((int16_t) (-50))) || (I(int16_t,8) < ((int16_t) (-50)));
  (*inst).Pumpensteuerung_P6 = F(int16_t,18) >= R(int16_t,4);
  (*inst).Pumpensteuerung_P7 = F(int16_t,10) > 3270;
  (*inst).Pumpensteuerung_P8 = F(int16_t,18) >= R(int16_t,6);
  (*inst).Pumpensteuerung_P9 = F(int16_t,18) >= 180;
  (*inst).Pumpensteuerung_P10 = I(int16_t,8) > R(int16_t,8);
  (*inst).Pumpensteuerung_P11 = (I(int16_t,8) + 2) > I(int16_t,14);
  (*inst).Pumpensteuerung_P12 = I(int16_t,8) < R(int16_t,10);
  if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 0)
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
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 1)
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
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 2)
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 3)
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (1));
  }
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 4)
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 5)
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
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 6)
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 7)
  {
    if (!IX(0,0))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
    }
  }
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 8)
  {
    if (!IX(0,1))
    {
      (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
    }
  }
  else if (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 9)
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (0));
  }
  else
  {
    (*inst).Pumpensteuerung_Op0 = ((uint8_t) (4));
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
  QX(0,0) = (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 1);
  /* Störung Sensor Pumpenschacht */
  FX(0,1) = (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 7);
  /* Störung Sensor Filtergraben */
  FX(0,2) = (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 8);
  /* Zeitdifferenz berechnen */
  _OV10 = (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 0) || (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 1);
  /* Zeitverhältnis mitteln */
  _OV6 = (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 2) || (((int64_t) ((*inst).Pumpensteuerung_Op0))
   == 6) || (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 9);
  /* Zeit merken */
  _OV7 = (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 3) || (((int64_t) ((*inst).Pumpensteuerung_Op0)) == 4) || _OV6;
  if ((((int64_t) ((*inst).Pumpensteuerung_Op0)) == 5))
  {
    F(int16_t,10) = F(int16_t,4) - (*inst).time_last;
  }
  if ((((int64_t) ((*inst).Pumpensteuerung_Op0)) == 3))
  {
    F(int16_t,12) = F(int16_t,4) - (*inst).time_last;
  }
  if ((((int64_t) ((*inst).Pumpensteuerung_Op0)) == 2))
  {
    F(int16_t,14) = ((int16_t) ((F(int16_t,10) * 100) / (F(int16_t,10) + F(int16_t,12))));
  }
  if ((((int64_t) ((*inst).Pumpensteuerung_Op0)) == 6))
  {
    F(int16_t,14) = ((int16_t) ((F(int16_t,10) * 10) / (F(int16_t,10) / 10 + F(int16_t,12) / 10)));
  }
  if ((((int64_t) ((*inst).Pumpensteuerung_Op0)) == 9))
  {
    F(int16_t,14) = ((int16_t) (F(int16_t,10) / (F(int16_t,10) / 100 + F(int16_t,12) / 100)));
  }
  if (_OV6)
  {
    F(int16_t,16) = ((int16_t) ((F(int16_t,16) * 4) / 5 + F(int16_t,14) / 5));
  }
  if (_OV7)
  {
    (*inst).time_last = F(int16_t,4);
  }
  if (_OV10)
  {
    F(int16_t,18) = F(int16_t,4) - (*inst).time_last;
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

#endif

#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Solaranlage_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Solaranlage_st

// BEGIN_BLOCK Solaranlage
struct Solaranlage {
  /* VAR */
  /* Operation 0 aktiv*/
  uint8_t Solaranlage_Op0;
  /* Aussentemperatur > 4°C*/
  bool Solaranlage_P0;
  /* Solartemperatur > Teichtemperatur*/
  bool Solaranlage_P1;
  int16_t z;
};
void Solaranlage (
  struct Solaranlage *inst
  )
{
  /* Automatisch generiert von TeCAD, bitte Änderungen nur dort vornehmen.*/
  /* Titel: Solaranlage */
  /* Author: TeCAD */
  if (!FX(0,0))
  {
    (*inst).Solaranlage_Op0 = ((uint8_t) (2));
  }
  /* Initialisierung der Operationsvariablen */
  /* Berechnung der nicht binären Prozeßvariablen */
  (*inst).Solaranlage_P0 = I(int16_t,12) > 40;
  (*inst).Solaranlage_P1 = I(int16_t,10) > I(int16_t,8);
  if (((int64_t) ((*inst).Solaranlage_Op0)) == 0)
  {
    if (((*inst).Solaranlage_P0 && (*inst).Solaranlage_P1))
    {
      (*inst).Solaranlage_Op0 = ((uint8_t) (1));
    }
  }
  else if (((int64_t) ((*inst).Solaranlage_Op0)) == 1)
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
  /* Ausgabe der Operationsübergänge */
  /* Auswahl der Operation 0 */
  /* Übergang von Operation 0 zu Operation 1 */
  /* Auswahl der Operation 1 */
  /* Übergang von Operation 1 zu Operation 0 */
  /* Initialisierung wenn keine Operation gesetzt ist */
  /* Zuweisung der Operationsvariablen */
  /* Magnetventil Fischwasser einschalten */
  QX(0,2) = (((int64_t) ((*inst).Solaranlage_Op0)) == 1);
  if ((((int64_t) ((*inst).Solaranlage_Op0)) == 0))
  {
    (*inst).z = F(int16_t,4);
  }
  /* Ausgabe Sonderfunktionen und Timeraufrufe */
  /* Zeit merken */
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

#endif

#ifndef INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Zyklus_st
#define INC_GUARD_C__daten_annualweekendcoding_projects_Teichsteuerung_Teichsteuerung_MC1_Zyklus_Zyklus_st

// BEGIN_BLOCK Zyklus
struct Zyklus {
  /* VAR */
  struct Pumpensteuerung I_Pumpensteuerung;
  struct Solaranlage I_Solaranlage;
  struct Temperatursensoren I_Temperatursensoren;
};
void Zyklus (
  struct Zyklus *inst
  )
{
  /* automatisch generiert von TERANiS-Studio, bitte Änderungen nur dort vornehmen.*/
  /* Titel: Zyklus */
  /* Author: TERANiS */
  akttimes();
  Pumpensteuerung(&((*inst).I_Pumpensteuerung));
  Solaranlage(&((*inst).I_Solaranlage));
  Temperatursensoren(&((*inst).I_Temperatursensoren));
  /* akttimes.st */
  /* Pumpensteuerung.prp */
  /* Solaranlage.prp */
  /* Temperatursensoren.prp */
  /* Initialisierungsvariable setzen */
  FX(0,0) = true;
}
// END_BLOCK

#endif

