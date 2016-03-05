FUNCTION_BLOCK PIR // PI-Regler (Potential-Integral-Regler) - sollte im Zeit OB aufgerufen werden. 
{ #DEFINE TITLE "PI-Regler" }  
{ #DEFINE AUTHOR "PaulA" }  
{ #DEFINE VERSION "1.1" }
// Der PI-Regler kann durch Ki=0 die Funktion eines P-Reglers übernehmen.
// Die Konstanten Kd und Ki sind ensprechend zu parametrieren.
// Dafür kann das Tabellendokument "PID-Regler.ods" genutzt werden.
// Kd und Ki sind in SI-Einheiten anzugeben.
// AG setzt den Ausgang auf den Abgleichwert AGW.
// Bei Stpp/Stpn wird der Ausgabewert in entsprechender Richtung
// nicht weiter integriert.
// Wird der Sollwert einmal erreicht, kann der Istwert innerhalb der 
// Hysterese schwanken, ohne eine weitere Regelung hervorzurufen.
// Die Übertragungsfunktion hat die Form:
// Y = Kp (W(t) + Ki * Integral(W(t) dt))
  VAR_INPUT
    X : REAL ;			            // Istwert
    W : REAL ;						// Sollwert
    KP : REAL  := 1.0 ;				// Konstante Proportional-Anteil
    KI : REAL ;						// Konstante Integral-Anteil = 1/Tn [1/s]
    UG : REAL ;						// Untergrenze der Wind-Up-Behandlung
    OG : REAL  := 100.0 ;			// Obergrenze der Wind-Up-Behandlung
    HYST : REAL  := 1.0 ;			// Abweichung die ohne Regelung auftreten darf
    AGW : REAL ;					// Abgleichwert
    SG : REAL ;						// Störgröße
    AG : BOOL  := TRUE ;			// Abgleich Y auf Abgleichwert wenn AG/Handbetrieb
    STPP : BOOL ;					// Integratorstop positiv
    STPN : BOOL ;					// Integratorstop negativ
	C : TIME := TIME#1s ;				// "Konstante" die die Zyklusdauer speichern soll
  END_VAR
  VAR_IN_OUT
    Y : REAL ;						// Ausgang
  END_VAR
  VAR
  XWALT : REAL := 0.0; // alte Regelabweichung @IN
  SGALT : REAL := 0.0; // alte Störgröße @IN
  YSUM : REAL := 0.0; // speichert den letzten Ausganswert und dient zur Berechnung @IN
  HYSTSCH : BOOL := FALSE; // Schalter, der nur bei Regelabweichung = 0 wirksam wird @IN
									// und die Regelabweichung in der Hysterese als 0 bewertet
  END_VAR
  VAR_TEMP
    XW : REAL ;			// Differenz zwischen Soll und Ist
    UGWU : BOOL ;		// Unterer Grenzwert unterschritten
    OGWU : BOOL ;		// Oberer Grenzwert überschritten
    TEMP : REAL ;		// dient der Berechnung des Integrations-Anteils und Zwischenspeicherung
    CTEMP : REAL ;
  END_VAR

// Umrechnung von c in eine Real-Variable
  CTEMP := DINT_TO_REAL(TIME_TO_DINT(C))/1000.0 ;
// Vorverarbeitung von XW
  XW := W - X ;                // Soll - Ist

// Auswertung der Ist-Werte bezüglich der Hysterese
  IF ABS (XW) > HYST THEN
    HYSTSCH := FALSE ;
// Eingangshysterese
// die Regelabweichung schwingt sich so lange ein, bis sie den Wert 0 erreicht
  ELSIF XW = 0 THEN
    HYSTSCH := TRUE ;
// dann bleibt sie auf 0, solange sie in der Hysterese liegt
  ELSIF HYSTSCH THEN
    XW := 0 ;
  END_IF ;
    
  IF STPP & NOT (XW < 0) THEN		// Integratorstop positiv?
    TEMP := 1 ;		// kein Integrationsteil
  ELSIF STPN & NOT (XW > 0)	THEN	// Integratorstop negativ?
    TEMP := 1 ;		// kein Integrationsteil
  ELSE
    TEMP := KI * CTEMP + 1 ;	// Integrationsteil wird wirksam
  END_IF ;

// Abgleich, wenn Hand
  IF AG	THEN 
    YSUM := AGW ;
  ELSE 
    YSUM := YSUM + (TEMP * XW - XWALT) * KP ; 	// neuer Stellwert
    // Störgrößenaufschaltung
    TEMP := SG - SGALT + YSUM ;
	// Integralteilberechnung
    // mit Wind-Up Berücksichtigung (Rückrechnung der Summe)
    // bei Wind-up Ausgang halten
	YSUM := TEMP ;
  END_IF ;
  XWALT := XW ; 	// muss im nächsten Durchlauf zur Verfügung stehen
  SGALT := SG ;		// muss im nächsten Durchlauf zur Verfügung stehen

// Begrenzung und Zuweisung des Ausgangswertes und des internen Integrators
  UGWU := (TEMP <= UG) ;	// Untergrenze
  OGWU := (TEMP >= OG) ;	// Obergrenze
  IF UGWU THEN
    Y := UG ;
    YSUM := UG ;
  ELSIF OGWU THEN
    Y := OG ;
    YSUM := OG ;
  ELSE
    Y := YSUM ;
  END_IF ;

END_FUNCTION_BLOCK
