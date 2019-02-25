#ifndef _INC_PLATFORM_H

#ifdef ARDUINO
  #define STRING String
#else
  #if defined(__BORLANDC__)
    #include <vcl.h>
    #pragma hdrstop
  #endif
  #include <string>
  #define STRING std::string
#endif

#if !defined(__BORLANDC__)
  class Exception {
    public:
      STRING Message;
      Exception(STRING message) {Message=message;};
   };
#endif

class TMException : public Exception
{
  public:
    TMException(STRING message) : Exception(message.c_str()) {};
    TMException(STRING message, int param1) : Exception(message.c_str()) {};
    TMException(STRING message, int param1, int param2) : Exception(message.c_str()) {};
};

#endif
