//---------------------------------------------------------------------------

#ifndef MRegExpH
#define MRegExpH

#if defined(__BORLANDC__) && !defined(__clang__) && defined(_WIN32)
  #define PERLREGEX
#else
  #define STDREGEX
  #include <regex>
#endif

#ifdef STDREGEX
  #include <regex>
#endif

#ifdef PERLREGEX
  #include <regexp.h>
  #include <pcreposi.h>
#endif

#include <string>
#include <vector>

using std::string;
using std::vector;

class TMRegExp 
{
  private:
    #ifdef STDREGEX
      std::regex aRegExp;
      std::smatch aRegMatch;
      int maxmatch;
    #endif
    #ifdef PERLREGEX
      regex_t aRegExp;
      regmatch_t *aRegMatch;
    #endif
    string FText;
    string FExpression;
    bool matched;
    bool compiled;
    TMRegExp( const TMRegExp &other) {};
      //; Copy-Konstruktor private, damit Klasse nicht kopiert werden kann
  public:
    bool SetExpression(string value, int maxmatch = 0);
      //: setzt einen neuen regul�ren Ausdruck
      //. bei Fehler wird eine Exception erzeugt bzw. bei _NO_EXEPTOBJ false zur�ckgeliefert
    string GetExpression();
    TMRegExp(int maxmatch = 0);
    TMRegExp(string Expression, int maxmatch = 0);
      //: setzt einen neuen regul�ren Ausdruck
      //+ Wenn maxmatch = 0 angegeben wird, werden die Klammern gez�hlt aber mindestens 10 gesetzt
      //. bei Fehler wird eine Exception erzeugt
    ~TMRegExp();
    bool Exec(string text);
      //: f�hrt den Text mit dem aktuellen regul�ren Ausdruck aus
      //+ @param text Eingabetext
      //+ @return true wenn der Text zu dem Ausdruck passt
      //. Wenn true geliefert wurde k�nnen die Matches gelesen werden
    bool Exec(string text, vector<string> &list);
      //: f�hrt den Text mit dem aktuellen regul�ren Ausdruck aus
      //+ @param text Eingabetext
      //+ @param list  Referenz auf eine Stringliste zu der die Matches hinzugef�gt werden sollen
      //+ @return true wenn der Text zu dem Ausdruck passt
      //. Wenn true geliefert wurde k�nnen die Matches gelesen werden
    bool ExecExpression(string Expression, string text);
      //: setzt den �bergebenen Ausdruck
      //+ f�hrt den Text mit dem �bergebenen regul�ren Ausdruck aus
      //+ @param expression regul�rer Ausdruck
      //+ @param text Eingabetext
      //+ @return true wenn der Text zu dem Ausdruck passt
      //. Wenn true geliefert wurde k�nnen die Matches gelesen werden
    void GetMatches(vector<string> &list);
      //: liefert alle Matches in einer Liste
      //+ @param list Referenz auf eine Stringliste zu der die Matches hinzugef�gt werden sollen
      //+ Die Liste wird nicht gel�scht.
      //. Wenn Exec nicht true geliefert hat bleibt die Liste unber�hrt.
    string GetMatch(int num);
      //: liefert den Match mit der Nummer
      //+ @param num Nummer des Matchs beginnt bei 0
      //+ @return Match (Ein Substring aus dem vorher �bergebenen Text)
      //. Wenn Exec nicht true geliefert hat wird immer ein Leerstring zur�ckgeliefert
    string GetMatch(int num, int &pos);
      //: liefert den Match mit der Nummer
      //+ @param num Nummer des Matchs beginnt bei 0
      //+ @param pos Anfangspostion des gefundenen Strings
      //+ @return Match (Ein Substring aus dem vorher �bergebenen Text)
      //. Wenn Exec nicht true geliefert hat wird immer ein Leerstring zur�ckgeliefert
};

//---------------------------------------------------------------------------
#endif
