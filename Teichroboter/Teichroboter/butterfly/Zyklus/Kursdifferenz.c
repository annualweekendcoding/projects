
VOID Kursdifferenz (
)
{
if (kompass_grad < 0) {
/* Bei Kompassfehler die Differenz auf 0 setzen */
  kurs_diff = 0;
}
else {
  kurs_diff = kurs_akt - kompass_grad;
/* wenn die Differenz 180° übersteigt, dann in die andere Richtung negativ setzen */
  if (kurs_diff > 1800) {
    kurs_diff = kurs_diff - 3600;
  }
  else if (kurs_diff < -1800) {
    kurs_diff = kurs_diff + 3600;
  }
}
}
