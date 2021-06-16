#ifndef PRIVATE_PARALLEL_HPP
#define PRIVATE_PARALLEL_HPP

#include <qtparallel.h>

/*!
 * The functions here are hidden and used only internally.
 * You should not use these functions outside this code project.
 *
 */

#if   defined(Q_OS_ANDROID)
#include <limits.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <fcntl.h>
#elif defined(Q_OS_IOS)
#include <limits.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <fcntl.h>
#elif defined(Q_OS_LINUX)
#include <limits.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <fcntl.h>
#elif defined(Q_OS_MACX)
#include <limits.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <fcntl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <direct.h>
#include <winsock2.h>
#include <process.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

QT_BEGIN_NAMESPACE
BEGIN_PARALLEL_NAMESPACE

void mpsleep(int msec) ;

class IpAddress : public QByteArray
{
  public:

    explicit     IpAddress   (void) ;
                 IpAddress   (const IpAddress & address) ;
                 IpAddress   (const char      * address) ;
                 IpAddress   (QString           address) ;
                 IpAddress   (std::string       address) ;
    virtual     ~IpAddress   (void) ;

    static bool  ipcmp       (const sockaddr * addr1, const sockaddr * addr2, int ver = AF_INET) ;
    static void  ntop        (const sockaddr * addr ,       quint32    ip[4], int ver = AF_INET) ;
    static void  pton        (      sockaddr * addr , const quint32    ip[4], int ver = AF_INET) ;

    virtual int  type        (void) ;
    virtual void setType     (int type) ;
    virtual int  Port        (void) ;
    virtual void setPort     (int port) ;

    #if defined(Q_OS_WIN)
    int          length      (void) ; // sizeof(sockaddr_in) or sizeof(sockaddr_in_ipv6)
    #else
    socklen_t    length      (void) ; // sizeof(sockaddr_in) or sizeof(sockaddr_in_ipv6)
    #endif

    bool         obtain      (struct sockaddr_in  & sockin) ;
    bool         obtain      (struct sockaddr_in6 & sockin) ;
    bool         obtain      (struct sockaddr_in  * sockin) ;

    QString      toString    (bool withPort = false) ;
    std::string  toStdString (bool withPort = false) ;

    bool         setAddress  (struct sockaddr     * address) ;
    bool         setAddress  (struct sockaddr_in  & address) ;
    bool         setAddress  (struct sockaddr_in6 & address) ;
    bool         setAddress  (const char          * address) ;
    bool         setAddress  (QString               address) ;
    bool         setAddress  (std::string           address) ;

  protected:

  private:

} ;

typedef QList<IpAddress> IpAddresses ;

bool Lookup (QString hostname,IpAddresses & addresses,QString service = "") ;

/*! Internal socket utilities */

#ifdef Q_OS_WIN

SOCKET GetUdpSocket (void) ;
SOCKET GetTcpSocket (void) ;

bool   SetNonblock      (SOCKET socket) ;
bool   SetReuseAddress  (SOCKET socket) ;

int    GetReceiveBuffer (SOCKET socket) ;
int    GetSendingBuffer (SOCKET socket) ;
bool   SetReceiveBuffer (SOCKET socket,int buffersize) ;
bool   SetSendingBuffer (SOCKET socket,int buffersize) ;

#else

int    GetUdpSocket     (void) ;
int    GetTcpSocket     (void) ;

bool   SetNonblock      (int    socket) ;
bool   SetReuseAddress  (int    socket) ;

int    GetReceiveBuffer (int    socket) ;
int    GetSendingBuffer (int    socket) ;
bool   SetReceiveBuffer (int    socket,int buffersize) ;
bool   SetSendingBuffer (int    socket,int buffersize) ;

#endif

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE

#endif
