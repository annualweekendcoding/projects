FUNCTION akttimes : void
VAR_EXTERNAL
  now AT %I(int64_t,20) : LINT; // Ticks in Millisekunden seit Start @ID=25 @IN
  time_ms AT %F(int16_t,2) : INT; // Ticks in Sekunden als Umlaufzähler @ID=27 @INOUT
  time_sec AT %F(int16_t,4) : INT; // Ticks in Sekunden als Umlaufzähler @ID=9 @INOUT
  lastNow AT %F(int64_t,60) : LINT; // gemerkte Ticks @ID=28 @LOCAL @INOUT
  lastMs AT %F(int16_t,6) : INT; // gemerkte ms @ID=29 @LOCAL @INOUT
END_VAR
VAR_TEMP
  l : LINT;
  i : INT;
END_VAR
  l := now-lastNow;
  if l>0 AND l<10000 then
    // Millisekunden Ticker
    time_ms := time_ms + l;
  end_if;
  lastNow :=now;
  
  i := time_ms-lastMs;
  if i<0 then
    lastMs := time_ms;
  elsif i>1000 then
    time_sec := time_sec + i/1000;
    lastMs := lastMs + (i/1000)*1000;  
  end_if;
END_FUNCTION