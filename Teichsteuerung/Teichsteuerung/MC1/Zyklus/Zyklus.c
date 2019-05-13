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
