
struct PIR {
/* VAR_INPUT */
/* Istwert*/
REAL X;
/* Sollwert*/
REAL W;
/* Konstante Proportional-Anteil*/
REAL KP;
/* Konstante Integral-Anteil = 1/Tn [1/s]*/
REAL KI;
/* Untergrenze der Wind-Up-Behandlung*/
REAL UG;
/* Obergrenze der Wind-Up-Behandlung*/
REAL OG;
/* Abweichung die ohne Regelung auftreten darf*/
REAL HYST;
/* Abgleichwert*/
REAL AGW;
/* Störgröße*/
REAL SG;
/* Abgleich Y auf Abgleichwert wenn AG/Handbetrieb*/
BOOL AG;
/* Integratorstop positiv*/
BOOL STPP;
/* Integratorstop negativ*/
BOOL STPN;
/* "Konstante" die die Zyklusdauer speichern soll*/
TIME C;
/* VAR */
/* alte Regelabweichung @IN*/
REAL XWALT;
/* alte Störgröße @IN*/
REAL SGALT;
/* speichert den letzten Ausganswert und dient zur Berechnung @IN*/
REAL YSUM;
/* Schalter, der nur bei Regelabweichung = 0 wirksam wird @IN*/
BOOL HYSTSCH;
};

VOID PIR(struct PIR *inst,
/* VAR_IN_OUT */
/* Ausgang*/
REAL *Y)
{
/* PI-Regler (Potential-Integral-Regler) - sollte im Zeit OB aufgerufen werden. */
/* Der PI-Regler kann durch Ki=0 die Funktion eines P-Reglers übernehmen.*/
/* Die Konstanten Kd und Ki sind ensprechend zu parametrieren.*/
/* Dafür kann das Tabellendokument "PID-Regler.ods" genutzt werden.*/
/* Kd und Ki sind in SI-Einheiten anzugeben.*/
/* AG setzt den Ausgang auf den Abgleichwert AGW.*/
/* Bei Stpp/Stpn wird der Ausgabewert in entsprechender Richtung*/
/* nicht weiter integriert.*/
/* Wird der Sollwert einmal erreicht, kann der Istwert innerhalb der */
/* Hysterese schwanken, ohne eine weitere Regelung hervorzurufen.*/
/* Die Übertragungsfunktion hat die Form:*/
/* Y = Kp (W(t) + Ki * Integral(W(t) dt))*/
/* und die Regelabweichung in der Hysterese als 0 bewertet*/

/* VAR_TEMP */
/* Differenz zwischen Soll und Ist*/
REAL XW;
/* Unterer Grenzwert unterschritten*/
BOOL UGWU;
/* Oberer Grenzwert überschritten*/
BOOL OGWU;
/* dient der Berechnung des Integrations-Anteils und Zwischenspeicherung*/
REAL TEMP;
REAL CTEMP;

/* Titel: PI-Regler */
/* Author: PaulA */
/* Version: 1.1 */

/* Umrechnung von c in eine Real-Variable */
CTEMP = DINT_TO_REAL(TIME_TO_DINT((*inst).C)) / 1000.0;
/* Vorverarbeitung von XW */
/* Soll - Ist */
XW = (*inst).W - (*inst).X;
/* Auswertung der Ist-Werte bezüglich der Hysterese */
if (ABS(XW) > (*inst).HYST) {
  (*inst).HYSTSCH = FALSE;
}
/* Eingangshysterese */
/* die Regelabweichung schwingt sich so lange ein, bis sie den Wert 0 erreicht */
else if (XW == 0.0) {
  (*inst).HYSTSCH = TRUE;
}
/* dann bleibt sie auf 0, solange sie in der Hysterese liegt */
else if ((*inst).HYSTSCH) {
  XW = 0.0;
}
/* Integratorstop positiv? */
if ((*inst).STPP && !(XW < 0.0)) {
/* kein Integrationsteil */
  TEMP = 1.0;
}
/* Integratorstop negativ? */
else if ((*inst).STPN && !(XW > 0.0)) {
/* kein Integrationsteil */
  TEMP = 1.0;
}
else {
/* Integrationsteil wird wirksam */
  TEMP = (*inst).KI * CTEMP + 1.0;
}
/* Abgleich, wenn Hand */
if ((*inst).AG) {
  (*inst).YSUM = (*inst).AGW;
}
else {
/* neuer Stellwert */
  (*inst).YSUM = (*inst).YSUM + (TEMP * XW - (*inst).XWALT) * (*inst).KP;
/* Störgrößenaufschaltung */
  TEMP = (*inst).SG - (*inst).SGALT + (*inst).YSUM;
/* Integralteilberechnung */
/* mit Wind-Up Berücksichtigung (Rückrechnung der Summe) */
/* bei Wind-up Ausgang halten */
  (*inst).YSUM = TEMP;
}
/* muss im nächsten Durchlauf zur Verfügung stehen */
(*inst).XWALT = XW;
/* muss im nächsten Durchlauf zur Verfügung stehen */
(*inst).SGALT = (*inst).SG;
/* Begrenzung und Zuweisung des Ausgangswertes und des internen Integrators */
/* Untergrenze */
UGWU = (TEMP <= (*inst).UG);
/* Obergrenze */
OGWU = (TEMP >= (*inst).OG);
if (UGWU) {
  (*Y) = (*inst).UG;
  (*inst).YSUM = (*inst).UG;
}
else if (OGWU) {
  (*Y) = (*inst).OG;
  (*inst).YSUM = (*inst).OG;
}
else {
  (*Y) = (*inst).YSUM;
}
}
