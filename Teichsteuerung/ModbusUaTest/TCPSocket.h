//---------------------------------------------------------------------------
#ifndef TCPSocketH
#define TCPSocketH

#include <string>

#include "MSocket.h"
//---------------------------------------------------------------------------

class TTCPSocket : public TMSocket
{
private:
  SOCKET descriptor;
  bool isReady;
  int port;
  std::string ip;
//  bool readable();
public:
  TTCPSocket();
  TTCPSocket(std::string hostport);
  TTCPSocket(int port, std::string host);
  ~TTCPSocket();
  bool Reconnect();
  bool Connect(std::string hostport);
  bool Connect(int port, std::string host);
  void Disconnect();
  /// Sendet auf einem Puffer die Anzahl Zeichen
  void Send(const void* buffer, unsigned int bufferlen);
  /// liest maximal die Pufferlänge und liefert die gelesene Anzahl zurück, nicht blockierend
  int Recv(char* buffer, unsigned int bufferlen);
  /// Sendet einen String
  void Send(std::string s);
  /// liest alles was im Puffer ist, in einen String
  std::string Recv();
  /// wirft alles was im Puffer ist weg
  void Flush();
  /// liest die angegebene Anzahl Zeichen
  /// dafür wird Recv wiederholt aufgerufen und wait_time dazwischen gewartet
  /// Der Aufruf kehrt erst zurück wenn die Anzahl Zeichen gelesen wurde
  int RecvFixed(char* buffer, unsigned int bufferlen, int wait_time, int wait_count);
  bool IsReady();
};
#endif
