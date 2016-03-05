/*
 * teranis.c
 *
 * Created: 04.06.2013 20:51:56
 *  Author: mirko
 */ 

#include <avr/io.h>
#include "stdefs.h"
#include "globalvar.h"

#include "..\..\Teichroboter\Bibliothek\PIR.c"

#include "..\..\Teichroboter\butterfly\Zyklus\Kursdifferenz.c"
#include "..\..\Teichroboter\butterfly\Zyklus\Motorsteuerung.c"
//#include "..\..\Teichroboter\butterfly\Zyklus\Takt_100ms.c"

#include "..\..\Teichroboter\butterfly\Zyklus\Akkustand.c"
#include "..\..\Teichroboter\butterfly\Zyklus\Antriebsfreigabe.c"
#include "..\..\Teichroboter\butterfly\Zyklus\Kurs_fahren.c"
#include "..\..\Teichroboter\butterfly\Zyklus\Pumpensteuerung.c"
#include "..\..\Teichroboter\butterfly\Zyklus\Zyklus.c"

struct Zyklus Zyklus_inst;

void teranis_zyklus()
{
  Zyklus(&Zyklus_inst);  
}

//struct Takt_100ms Takt_100ms_inst;

void teranis_takt_100ms()
{
  //Takt_100ms(&Takt_100ms_inst);
}
