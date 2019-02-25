#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>
#include <windows.h>
#include <iostream>

extern "C"{
  #include "teranis.h"
};

int _tmain(int argc, _TCHAR* argv[])
{
  setup_teranis();

  int old=0;

  while(true)
  {
    // now übertragen
    IREF(int64_t,20) = GetTickCount64();

    loop_teranis();

    if (QX(0,1))
    {
      std::cout << (FINT(2)) << std::endl;
    }
    if (old!=FINT(4))
    {
     // Serial.println(FINT(2));
      std::cout << (FINT(8)) << std::endl;
      old=FINT(4);
    }
  }
  return 0;
}
