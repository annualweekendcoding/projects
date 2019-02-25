//---------------------------------------------------------------------------
#include "platform.h"

#include <string>

#if (_Windows)
  #include <io.h>
#else
  #include <sys/io.h>
  #include <cstring>
  #include <fcntl.h>
#endif

#include "TCPSocket.h"

//---------------------------------------------------------------------------
TTCPSocket::TTCPSocket()
{
  this->isReady = false;
  descriptor=0;
  port=0;
}
//---------------------------------------------------------------------------
TTCPSocket::TTCPSocket(std::string hostport)
{
  Connect(hostport);
}
//---------------------------------------------------------------------------
TTCPSocket::TTCPSocket(int port, std::string host)
{
  Connect(port, host);
}
//---------------------------------------------------------------------------
TTCPSocket::~TTCPSocket()
{
  Disconnect();
}
//---------------------------------------------------------------------------
void TTCPSocket::Disconnect()
{
  isReady = false;
  #if (_Windows)
    closesocket(descriptor);
  #else
    close(descriptor);
  #endif
  descriptor=0;
}
//---------------------------------------------------------------------------
bool TTCPSocket::Connect(std::string hostport)
{
  int port;
  std::string host;
  Hostport2HostPort(hostport,host,port);
  return Connect(port,host);
}
//---------------------------------------------------------------------------
bool TTCPSocket::Connect(int port, std::string host)
{
  Disconnect();
  SOCKADDR_IN addr;
  this->port = port;
  this->ip = Host2Ip(host);
  this->descriptor = socket(AF_INET,SOCK_STREAM,0);
  #if (_Windows)
    if(this->descriptor==INVALID_SOCKET) return false;
  #else
    if(this->descriptor<0) return false;
  #endif

  std::memset(&addr,0,sizeof(SOCKADDR_IN)); // zuerst alles auf 0 setzten
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(port);

  #if (_Windows)
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
  #else
    struct hostent *server = gethostbyname(ip.c_str());
    if (server == NULL) return false;
    bcopy((char *)server->h_addr,(char *)&addr.sin_addr.s_addr,server->h_length);
  #endif

  this->isReady = (-1 != connect(this->descriptor,(SOCKADDR*)&addr,sizeof(addr)));
  // Socket in nonblocking schalten
  #if (_Windows)
    unsigned long ctl = 1;
    if (ioctlsocket(this->descriptor,FIONBIO,&ctl)) return false;
  #else
    int flags;
    if ((flags = fcntl(this->descriptor, F_GETFL, 0)) < 0) return false;
    if (fcntl(this->descriptor, F_SETFL, flags | O_NONBLOCK) < 0) return false;
  #endif
  return IsReady();
}
//---------------------------------------------------------------------------
bool TTCPSocket::Reconnect()
{
  Disconnect();
  return Connect(this->port, this->ip);
}
//---------------------------------------------------------------------------
void TTCPSocket::Send(const void* buffer, unsigned int bufferlen)
{
  send(this->descriptor,(const char*)buffer,bufferlen,0);
}
//---------------------------------------------------------------------------
int TTCPSocket::Recv(char* buffer, unsigned int bufferlen)
{
  int result = recv(this->descriptor,buffer,bufferlen,0);
  #if (_Windows)
    // Wenn nur keine Daten, dann einfach 0 zurückliefern
    if (result==SOCKET_ERROR && WSAGetLastError()==WSAEWOULDBLOCK) return 0;
  #else
    // Wenn nur keine Daten, dann einfach 0 zurückliefern
    if (result<0 && (errno==EAGAIN || errno==EWOULDBLOCK)) return 0;
  #endif
  return result;
}
//---------------------------------------------------------------------------
void TTCPSocket::Send(std::string s)
{
  // Stringlänge abschicken
  Send(s.c_str(),s.length());
}
//---------------------------------------------------------------------------
std::string TTCPSocket::Recv()
{
  // alles was sich im Empfangspuffer befindet in einen String auslesen
  std::string result;
  while (true)
  {
    char buffer[255];
    for (int i=0; i<254; i++) buffer[i]=0;
    int len = Recv(buffer,253);
    result += buffer;
    if (len<253) break;
  }
  return result;
}
//---------------------------------------------------------------------------
void TTCPSocket::Flush()
{
  // alles was sich im Empfangspuffer befindet auslesen
  while (true)
  {
    char buffer[255];
    for (int i=0; i<254; i++) buffer[i]=0;
    int len = Recv(buffer,253);
    if (len<253) break;
  }
}
//---------------------------------------------------------------------------
int TTCPSocket::RecvFixed(char* buffer, unsigned int bufferlen, int wait_time, int wait_count)
{
  int rc;
  unsigned int read = 0;
  int i = 0;
  while(read < bufferlen && rc != -1)
  {
    rc = Recv(buffer+read,bufferlen);
    if(rc==-1)throw Exception("socket error");
    else read += rc;
    if(read<bufferlen){
      if(++i>wait_count) throw TMException("socket error : can't receive enough data : ? / ?",read,bufferlen);
      #if (_Windows)
        Sleep(wait_time);
      #else
        usleep(wait_time*1000);
      #endif
    }
  }
  return read;
}
//---------------------------------------------------------------------------
bool TTCPSocket::IsReady()
{
  return this->isReady;
}
//---------------------------------------------------------------------------
/*bool TTCPSocket::readable()
{
    FD_SET fdSet;
    TIMEVAL timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    long status;

    FD_ZERO(&fdSet);
    FD_SET(this->descriptor,&fdSet);
    status = select(0,&fdSet,0,0,&timeout);
    if(status <= 0) FD_ZERO(&fdSet);
    if(!FD_ISSET(this->descriptor,&fdSet))  return false;
    return true;
}
*/
//---------------------------------------------------------------------------
