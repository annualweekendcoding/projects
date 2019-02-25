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
