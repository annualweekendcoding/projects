#include <iostream>
#include <iomanip>
#include "SunRiseSet.h"

using namespace std;
//using std::setw;

int main()
{
    TSunRiseSet sunRiseSet(12.7297147,54.33705948,1);
    float Aufgang, Untergang;
    sunRiseSet.Get(2020,2,24,Aufgang,Untergang);
    int16_t AufgangStunden, AufgangMinuten;
    sunRiseSet.HourMinute(Aufgang,AufgangStunden,AufgangMinuten);
    int16_t UntergangStunden, UntergangMinuten;
    sunRiseSet.HourMinute(Untergang,UntergangStunden,UntergangMinuten);

    cout << "Aufgang " << setfill('0') << setw(2) << AufgangStunden << ":" << AufgangMinuten << endl;
    cout << "Untergang "<< UntergangStunden << ":" << UntergangMinuten << endl;

    return 0;
}