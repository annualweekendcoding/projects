#include <ESP8266HTTPClient.h>


bool HMStateChange(int id, float value)
{
  // Einen Wert in der Homematic setzen
  // Dafür muss die ise_id bekannt sein. 
  char s[90];
  snprintf(s,90,"http://192.168.1.10//config/xmlapi/statechange.cgi?ise_id=%d&new_value=%f",id,value);
  HTTPClient homematic;
  homematic.begin(s);
  int httpCode = homematic.GET();
  homematic.end();
  return (httpCode>0);
}
