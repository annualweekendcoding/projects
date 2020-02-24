#include "time.h"

const char* ntpServer = "de.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
const unsigned long ntpInterval = 60 * 60 * 1000; // ms

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

unsigned long lastSync = 0;

void timeSync() {  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)) lastSync = millis();
}  


void timeSetup()
{
  timeSync();
  printLocalTime();
}

unsigned long lastTime = 0;

void timeLoop()
{
  if (lastTime==0 || millis()-lastTime>1000) {
    if (lastSync==0 || millis()-lastSync>ntpInterval) {
      timeSync();
    }
    lastTime = millis();
    //printLocalTime();
  }  
}
