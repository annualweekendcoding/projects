
struct Motorregler {
/* VAR */
/* @NODB*/
struct PIR PIR;
/* @NODB*/
REAL YR;
};

VOID Motorregler(struct Motorregler *inst)
{
/* VAR_TEMP */
BOOL aus;
/* geschwindigkeitsoffset  */
INT g;
INT y;

aus = anlage_aus || akku_niedrig;
(*inst).PIR.X = INT_TO_REAL(kurs_diff);
/* Sollwert */
(*inst).PIR.W = 0.0;
/* Konstante Proportional-Anteil  */
(*inst).PIR.KP = 0.05;
/* Konstante Integral-Anteil = 1/Tn [1/s]  */
(*inst).PIR.KI = 0.0;
/* Untergrenze der Wind-Up-Behandlung  */
(*inst).PIR.UG = -30.0;
/* Obergrenze der Wind-Up-Behandlung  */
(*inst).PIR.OG = 30.0;
/* Abweichung die ohne Regelung auftreten darf  */
(*inst).PIR.HYST = 10.0;
/* Abgleichwert  */
(*inst).PIR.AGW = INT_TO_REAL(kurs_diff);
/* Störgröße  */
(*inst).PIR.SG = 0.0;
/* Abgleich Y auf Abgleichwert wenn AG/Handbetrieb  */
(*inst).PIR.AG = aus;
/* Integratorstop positiv */
(*inst).PIR.STPP = FALSE;
/* Integratorstop negativ */
(*inst).PIR.STPN = FALSE;
/* "Konstante" die die Zyklusdauer speichern soll */
(*inst).PIR.C = DINT_TO_TIME(100);
PIR(&((*inst).PIR),&(*inst).YR);
if (aus) {
  rechts_pwm = 0;
  links_pwm = 0;
}
else {
  g = geschwindigkeit / 2;
  y = REAL_TO_INT((*inst).YR);
  if (geschwindigkeit > 0) {
/* vorwärts Kurs halten */
    g = g + 77;
    if (y > 0) {
/* rechter Motor mehr drehen um nach links zu drehen */
      rechts_pwm = g + y;
      links_pwm = g;
    }
    else {
/* linker Motor mehr drehen um nach rechts zu drehen */
      rechts_pwm = g;
      links_pwm = g - y;
    }
  }
  else if (geschwindigkeit < 0) {
/* rückwärts Kurs nach vorn halten */
    g = g - 77;
    if (y > 0) {
/* linken Motor mehr rückwärts um nach links zu drehen */
      rechts_pwm = g;
      links_pwm = g - y;
    }
    else {
/* rechten Motor mehr rückwärts um nach rechts zu drehen */
      rechts_pwm = g + y;
      links_pwm = g;
    }
  }
  else {
/* Geschwindigkeit 0 = auf der Stelle drehen   */
    if (y > 1) {
/* rechter Motor vor und linker Motor rück um nach links zu drehen */
      rechts_pwm = 77 + y;
      links_pwm = -77 - y / 2;
    }
    else if (y < -1) {
/* rechter Motor rück und linker Motor vor um nach rechts zu drehen */
      rechts_pwm = -77 + y;
      links_pwm = 77 - y;
    }
  }
}
}
