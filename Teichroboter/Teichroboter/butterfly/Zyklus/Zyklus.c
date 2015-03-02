
struct Zyklus {
/* VAR */
struct Akkustand I_Akkustand;
struct Antriebsfreigabe I_Antriebsfreigabe;
struct Kurs_fahren I_Kurs_fahren;
struct Motorsteuerung I_Motorsteuerung;
struct Pumpensteuerung I_Pumpensteuerung;
};

VOID Zyklus(struct Zyklus *inst)
{
/* automatisch generiert von TERANiS-Studio, bitte Änderungen nur dort vornehmen.*/

/* Titel: Zyklus */
/* Author: TERANiS */

/* Akkustand.prp */
Akkustand(&((*inst).I_Akkustand));
/* Antriebsfreigabe.prp */
Antriebsfreigabe(&((*inst).I_Antriebsfreigabe));
/* Kurs_fahren.prp */
Kurs_fahren(&((*inst).I_Kurs_fahren));
/* Kursdifferenz.st */
Kursdifferenz();
/* Motorsteuerung.prp */
Motorsteuerung(&((*inst).I_Motorsteuerung));
/* Pumpensteuerung.prp */
Pumpensteuerung(&((*inst).I_Pumpensteuerung));
/* Initialisierungsvariable setzen */
init = TRUE;
}
