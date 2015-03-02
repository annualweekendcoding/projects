{ #use <Bibliothek\PIR.st> }

FUNCTION_BLOCK Motorregler
VAR
  PIR : PIR; // @IN
  YR : REAL; // @IN
  RECHTS_PWM AT %rechts_pwm : INT; // Geschwindigkeit Motor rechts @IN
  LINKS_PWM AT %links_pwm : INT; // Geschwindigkeit Motor links @IN
  KURS_DIFF AT %kurs_diff : INT; // aktuelle Kursdifferenz zum Kompass @IN
  GESCHWINDIGKEIT AT %geschwindigkeit : INT; // aktuelle Sollgeschwindigkeit @IN
  ANLAGE_AUS AT %anlage_aus : BOOL; // Anlage AUS @IN
  AKKU_NIEDRIG AT %akku_niedrig : BOOL; // Akku niedrig @IN
END_VAR
VAR_TEMP
  aus : BOOL;
  g,y : INT; // geschwindigkeitsoffset  
END_VAR
  aus := ANLAGE_AUS OR AKKU_NIEDRIG;

  PIR.X := KURS_DIFF;
  PIR.W := 0;          // Sollwert
  PIR.KP :=0.05;				// Konstante Proportional-Anteil 
  PIR.KI :=0;					// Konstante Integral-Anteil = 1/Tn [1/s] 
  PIR.UG :=-30;					// Untergrenze der Wind-Up-Behandlung 
  PIR.OG :=30;			  // Obergrenze der Wind-Up-Behandlung 
  PIR.HYST :=10;			  // Abweichung die ohne Regelung auftreten darf 
  PIR.AGW := KURS_DIFF;	// Abgleichwert 
  PIR.SG :=0;					// Störgröße 
  PIR.AG := aus;	    // Abgleich Y auf Abgleichwert wenn AG/Handbetrieb 
  PIR.STPP := FALSE;		// Integratorstop positiv
  PIR.STPN := FALSE;		// Integratorstop negativ
  PIR.C := DINT_TO_TIME(100);	// "Konstante" die die Zyklusdauer speichern soll
  PIR(Y:=YR);  
  
  if aus then
    RECHTS_PWM := 0;
    LINKS_PWM := 0;
  else
    g := GESCHWINDIGKEIT/2;
    y := REAL_TO_INT(YR);
      
    if GESCHWINDIGKEIT>0 then
      // vorwärts Kurs halten
      g := g + 77;
      if y>0 then
        // rechter Motor mehr drehen um nach links zu drehen
        RECHTS_PWM := g + y;
        LINKS_PWM := g;
      else  
        // linker Motor mehr drehen um nach rechts zu drehen
        RECHTS_PWM := g;
        LINKS_PWM := g - y;
      end_if;
    elsif GESCHWINDIGKEIT<0 then
      // rückwärts Kurs nach vorn halten
      g := g - 77;
      if y>0 then
        // linken Motor mehr rückwärts um nach links zu drehen
        RECHTS_PWM := g;
        LINKS_PWM := g - y;
      else  
        // rechten Motor mehr rückwärts um nach rechts zu drehen
        RECHTS_PWM := g + y;
        LINKS_PWM := g;
      end_if;
    else
      // Geschwindigkeit 0 = auf der Stelle drehen  
      if y>1 then
        // rechter Motor vor und linker Motor rück um nach links zu drehen
        RECHTS_PWM := 77 + y;
        LINKS_PWM := -77 - y/2;
      elsif y<-1 then
        // rechter Motor rück und linker Motor vor um nach rechts zu drehen
        RECHTS_PWM := -77 + y;
        LINKS_PWM := 77 - y;
      end_if;  
    end_if; 
  end_if; 

END_FUNCTION_BLOCK   