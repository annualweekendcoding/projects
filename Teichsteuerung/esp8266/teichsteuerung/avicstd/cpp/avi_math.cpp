/* ---------------------------------------------------------------------------
* Copyright 2018 by AVI GmbH, Tim Trense
* ST-Definitions, expected to be available by TeCom-C.
* --------------------------------------------------------------------------- */

#include "avi_math.h"

double toRadians(double deg)
{
  return deg * MATH_PI / 180;
}

double toDegrees(double rad)
{
  return rad * 180 / MATH_PI;
}

#endif