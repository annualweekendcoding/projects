//
// Created by mirko on 24.02.2020.
//

#include "SunRiseSet.h"

float TSunRiseSet::julianDate(int16_t year, int16_t month, int16_t day) {
    if (month <= 2) {
        month += 12;
        year--;
    }
    int16_t Gregor = (year / 400) - (year / 100) + (year / 4);  // Gregorianischer Kalender
    return 2400000.5 + 365.0 * year - 679004.0 + Gregor
           + int16_t(30.6001 * (month + 1)) + day + 0.5; // für 12:00 des Tages berechnen
}

float TSunRiseSet::inPi(float x) {
    int16_t n = (int16_t) (x / PI2);
    x -= n * PI2;
    if (x < 0) x += PI2;
    return x;
}

float TSunRiseSet::eps(float T) {
    return RAD * (23.43929111 + (-46.8150 * T - 0.00059 * T * T + 0.001813 * T * T * T) / 3600.0);
}

float TSunRiseSet::calcTime(float &dk, float t) {

    float raM = 18.71506921
                + 2400.0513369 * t + (2.5862e-5 - 1.72e-9 * t) * t * t;

    float m = inPi(PI2 * (0.993133 + 99.997361 * t));
    float l = inPi(PI2 * (0.7859453 + m / PI2
                          + (6893.0 * sin(m) + 72.0 * sin(2.0 * m) + 6191.2 * t) / 1296.0e3));
    float e = eps(t);
    float ra = atan(tan(l) * cos(e));
    if (ra < 0.0) ra += PI1;
    if (l > PI1) ra += PI1;

    ra = 24.0 * ra / PI2;

    dk = asin(sin(e) * sin(l));

    // Damit 0<=RA_Mittel<24
    raM = 24.0 * inPi(PI2 * raM / 24.0) / PI2;

    float dRA = raM - ra;
    if (dRA < -12.0) dRA += 24.0;
    if (dRA > 12.0) dRA -= 24.0;

    dRA = dRA * 1.0027379;

    return dRA;
}

void TSunRiseSet::Get(int16_t year, int16_t month, int16_t day, float &sunrise, float &sunset) {
    const float JD2000 = 2451545.0; // Tage vom 1.1.2000
    float jd = julianDate(year, month, day);

    float t = (jd - JD2000) / 36525.0;
    float dk;
    float h = -50.0 / 60.0 * RAD;
    float B = latitude * RAD; // geographische Breite

    float time = calcTime(dk, t);
    float timediff = 12.0 * acos((sin(h) - sin(B) * sin(dk)) / (cos(B) * cos(dk))) / PI1;
    float sunriseLocal = 12.0 - timediff - time;
    float sunsetLocal = 12.0 + timediff - time;
    float sunriseUtc = sunriseLocal - longitude / 15.0;
    float sunsetUtc = sunsetLocal - longitude / 15.0;

    sunrise = sunriseUtc + timezone;         // In Stunden
    if (sunrise < 0.0) sunrise += 24.0;
    else if (sunrise >= 24.0) sunrise -= 24.0;

    sunset = sunsetUtc + timezone;
    if (sunset < 0.0) sunset += 24.0;
    else if (sunset >= 24.0) sunset -= 24.0;
}

void TSunRiseSet::HourMinute(float time, int16_t &hour, int16_t &minute) {
    minute = int16_t(60.0*(time - (int16_t)time)+0.5);
    hour = (int16_t) time;
    if (minute>=60) { minute-=60; hour++; }
    else if (minute<0) {
        minute+=60; hour--;
        if (hour<0) hour+=24;
    }
}
