//
// Created by mirko on 24.02.2020.
// based on https://lexikon.astronomie.info/zeitgleichung/neu.html
// Released into the public domain.
//

#ifndef SUNRISESET_H
#define SUNRISESET_H

#include <cmath>
#include <inttypes.h>

class TSunRiseSet {
private:
    const float PI1 = 3.1415926535897932384626433832795;
    const float PI2 = 2.0*PI1;
    const float RAD = PI1/180.0;

    float longitude;
    float latitude;
    int16_t timezone;

    /// Ermittelt das juliansche Datum ( Anzahl Tage )
    float julianDate(int16_t year, int16_t month, int16_t day);

    /// Bringt den Wert in den Bereich von PI
    float inPi(float x);

    /// Neigung der Erdachse
    float eps(float T);

    float calcTime(float &dk, float t);
public:
    TSunRiseSet(float longitude, float latitude, float zeitzone) :
            longitude(longitude), latitude(latitude), timezone(zeitzone) {}

    void Get(int16_t year, int16_t month, int16_t day, float &sunrise, float &sunset);

    /// Hilfsfunktion zur Umrechnung der Gleitpunktwerte in Stunden/Minuten
    static void HourMinute(float time, int16_t &hour, int16_t &minute);
};


#endif //SUNRISESET_H
