FUNCTION Kursdifferenz : VOID
VAR
  KOMPASS_GRAD AT %kompass_grad : INT; // Kompass Richtung, 0..3599, -1 bei Fehler @ID=7 @IN
  KURS AT %kurs_akt : INT; // aktuell gefahrener Kurs @ID=11 @IN
  KURS_DIFF AT %kurs_diff : INT; // aktuelle Kursdifferenz zum Kompass @ID=12 @IN
END_VAR
  if KOMPASS_GRAD<0 then
    // Bei Kompassfehler die Differenz auf 0 setzen
    KURS_DIFF := 0;
  else
    KURS_DIFF := KURS - KOMPASS_GRAD;
    // wenn die Differenz 180° übersteigt, dann in die andere Richtung negativ setzen
    if KURS_DIFF>1800 then
      KURS_DIFF := KURS_DIFF-3600;
    elsif KURS_DIFF<-1800 then
      KURS_DIFF := KURS_DIFF+3600;
    end_if;    
  end_if;  
END_FUNCTION