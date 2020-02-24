#include <SunRiseSet.h>
#include <time.h>

// Koordinaten und Zeitzone von Barth
TSunRiseSet sunRiseSet(12.7297147,54.33705948,1);
const float twilight = 2.0; // 2h Dämmerung

float sunrise=0,sunset=0;
uint8_t sunlevel=0;

void sonneLoop() {
  struct tm t;
  if (getLocalTime(&t)) {
    static uint8_t lastday=0;
    if (t.tm_mday!=lastday) {
      // einmal am Tag ausrechnen
      lastday = t.tm_mday;
      sunRiseSet.Get(t.tm_year+1900,t.tm_mon+1,t.tm_mday,sunrise,sunset);
      Serial.print("Sonnenaufgang: "); Serial.print(sunrise);
      Serial.print(" Sonnenuntergang: "); Serial.println(sunset);
    }
    static float time=0.0;
    //time = t.tm_hour + t.tm_min/60.0;

    static unsigned long lastTime = 0;
    if (lastTime==0 || millis()-lastTime>100) {
      lastTime = millis();
      time+=(1/60.0);
      if (time>24.0) time=0.0;
    
    if (time<sunrise-twilight || time>sunset+twilight) sunlevel = 0;
    else sunlevel = (1-abs((sunset+sunrise)/2-time)/((sunset-sunrise)/2+twilight))*255;  

    Serial.print("Time="); Serial.print(time); Serial.print(" Level="); Serial.println(sunlevel);
    }
  }
}

uint8_t getSunlevel() {
  return sunlevel;
}
