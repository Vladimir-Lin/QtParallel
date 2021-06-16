/****************************************************************************
 *
 * Copyright (C) 2016 Neutrino International Inc.
 *
 * Author   : Brian Lin ( Vladimir Lin , Vladimir Forest )
 * E-mail   : lin.foxman@gmail.com
 *          : lin.vladimir@gmail.com
 *          : wolfram_lin@yahoo.com
 *          : wolfram_lin@sina.com
 *          : wolfram_lin@163.com
 * Skype    : wolfram_lin
 * WeChat   : 153-0271-7160
 * WhatsApp : 153-0271-7160
 * QQ       : lin.vladimir@gmail.com (2107437784)
 * URL      : http://qtparallel.sourceforge.net/
 *
 * QtParallel acts as an interface for Qt to do parallel computing.
 *
 * Copyright 2001 ~ 2016
 *
 ****************************************************************************/

#include "parallel.hpp"

QT_BEGIN_NAMESPACE
BEGIN_PARALLEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////////

#pragma pack(push,1)

typedef struct           {
  int           type     ;
  int           port     ;
  unsigned char IP [ 4 ] ;
} IPv4address            ;

typedef struct           {
  int           type     ;
  int           port     ;
  quint32       IP [ 4 ] ;
} IPv6address            ;

#pragma pack(pop)

IpAddress:: IpAddress  (void)
          : QByteArray (    )
{
  setType ( AF_INET ) ;
}

IpAddress:: IpAddress  (const IpAddress & address)
          : QByteArray (                         )
{
  if ( address . size ( ) <= 0 )  return                   ;
  resize ( address . size ( ) )                            ;
  int             v =                   address . size ( ) ;
  unsigned char * s = (unsigned char *) address . data ( ) ;
  unsigned char * l = (unsigned char *)           data ( ) ;
  ::memcpy ( l , s , v )                                   ;
}

IpAddress:: IpAddress(const char * address)
{
  setAddress ( address ) ;
}

IpAddress:: IpAddress(QString address)
{
  setAddress ( address ) ;
}

IpAddress:: IpAddress(std::string address)
{
  setAddress ( address ) ;
}

IpAddress::~IpAddress(void)
{
}

bool IpAddress::ipcmp       (
        const sockaddr * addr1 ,
        const sockaddr * addr2 ,
        int              ver   )
{
  if ( NULL == addr1 ) return false                         ;
  if ( NULL == addr2 ) return false                         ;
  ///////////////////////////////////////////////////////////
  switch ( ver )                                            {
    case AF_INET                                            :
      if ( AF_INET == ver )                                 {
        struct sockaddr_in * a1 = (sockaddr_in *)addr1      ;
        struct sockaddr_in * a2 = (sockaddr_in *)addr2      ;
        if (( a1->sin_port        == a2->sin_port        ) &&
            ( a1->sin_addr.s_addr == a2->sin_addr.s_addr))  {
          return true                                       ;
        }                                                   ;
      }                                                     ;
    break;
    case AF_INET6:
      if ( AF_INET == ver )                                 {
        sockaddr_in6 * a1 = (sockaddr_in6 *)addr1           ;
        sockaddr_in6 * a2 = (sockaddr_in6 *)addr2           ;
        if ( a1->sin6_port == a2->sin6_port )               {
          for (int i = 0; i < 16; ++ i)                     {
            if (*((char*)&(a1->sin6_addr) + i)             !=
                *((char*)&(a2->sin6_addr) + i)            ) {
              return false                                  ;
            }                                               ;
          }                                                 ;
          return true                                       ;
        }                                                   ;
      }                                                     ;
    break                                                   ;
    default                                                 :
    break                                                   ;
  }                                                         ;
  return false                                              ;
}

void IpAddress::ntop(const sockaddr * addr,quint32 ip[4],int ver)
{
  switch ( ver )                                            {
    case AF_INET                                            :
      if ( AF_INET  == ver )                                {
        sockaddr_in * a = (sockaddr_in *) addr              ;
        ip[0] = a->sin_addr.s_addr                          ;
      }                                                     ;
    break                                                   ;
    case AF_INET6                                           :
      if ( AF_INET6 == ver )                                {
        sockaddr_in6 * a = (sockaddr_in6 *)addr             ;
        #define AIP(IPI,BASEX) ip[IPI]                    = \
          ( a -> sin6_addr . s6_addr [ BASEX + 3 ] << 24) + \
          ( a -> sin6_addr . s6_addr [ BASEX + 2 ] << 16) + \
          ( a -> sin6_addr . s6_addr [ BASEX + 1 ] <<  8) + \
            a -> sin6_addr . s6_addr [ BASEX     ]
        AIP ( 3 , 12 )                                      ;
        AIP ( 2 ,  8 )                                      ;
        AIP ( 1 ,  4 )                                      ;
        AIP ( 0 ,  0 )                                      ;
        #undef AIP
      }                                                     ;
    break                                                   ;
    default                                                 :
    break                                                   ;
  }                                                         ;
}

void IpAddress::pton(sockaddr * addr,const quint32 ip[4],int ver)
{
  switch ( ver )                                                           {
    case AF_INET                                                           :
      if ( AF_INET == ver )                                                {
        sockaddr_in  * a = (sockaddr_in  *)addr                            ;
        a->sin_addr.s_addr = ip[0]                                         ;
      } else                                                               ;
    break                                                                  ;
    case AF_INET6                                                          :
      if ( AF_INET == ver )                                                {
        sockaddr_in6 * a = (sockaddr_in6 *) addr                           ;
        #define ADRX(XX) a -> sin6_addr . s6_addr [ XX ]
        for (int i = 0; i < 4; ++ i)                                       {
          ADRX ( i * 4     ) = (unsigned char)( ip[i] & 0xFF             ) ;
          ADRX ( i * 4 + 1 ) = (unsigned char)((ip[i] & 0xFF00    ) >>  8) ;
          ADRX ( i * 4 + 2 ) = (unsigned char)((ip[i] & 0xFF0000  ) >> 16) ;
          ADRX ( i * 4 + 3 ) = (unsigned char)((ip[i] & 0xFF000000) >> 24) ;
        }                                                                  ;
      }                                                                    ;
    break                                                                  ;
    default                                                                :
    break                                                                  ;
  }                                                                        ;
}

int IpAddress::type(void)
{
  if ( size ( ) <= 0 ) return 0               ;
  IPv4address * ip = (IPv4address *) data ( ) ;
  return ip -> type                           ;
}

void IpAddress::setType(int type)
{
  IPv4address * ip4                              ;
  IPv6address * ip6                              ;
  switch ( type )                                {
    case AF_INET                                 :
      resize ( sizeof(IPv4address) )             ;
      ip4         = (IPv4address *) data ( )     ;
      ::memset ( ip4 , 0 , sizeof(IPv4address) ) ;
      ip4 -> type = AF_INET                      ;
    break                                        ;
    case AF_INET6                                :
      resize ( sizeof(IPv6address) )             ;
      ip6         = (IPv6address *) data ( )     ;
      ::memset ( ip6 , 0 , sizeof(IPv6address) ) ;
      ip6 -> type = AF_INET6                     ;
    break                                        ;
  }                                              ;
}

int IpAddress::Port(void)
{
  if ( size ( ) <= 0 ) return 0               ;
  IPv4address * ip = (IPv4address *) data ( ) ;
  return ip->port                             ;
}

void IpAddress::setPort(int port)
{
  if ( size ( ) <= 0 ) return                 ;
  IPv4address * ip = (IPv4address *) data ( ) ;
  ip -> port = port                           ;
}

QString IpAddress::toString(bool withPort)
{
  if (size()<=0) return ""                           ;
  QStringList   l                                    ;
  QString       s   = ""                             ;
  IPv4address * ip4 = (IPv4address *) data ( )       ;
  IPv6address * ip6 = (IPv6address *) data ( )       ;
  quint8      * ipf                                  ;
  switch ( ip4 -> type )                             {
    case AF_INET                                     :
      s   = QString ( "%1.%2.%3.%4"                  )
            .arg    ( ip4 -> IP [ 0 ]                )
            .arg    ( ip4 -> IP [ 1 ]                )
            .arg    ( ip4 -> IP [ 2 ]                )
            .arg    ( ip4 -> IP [ 3 ]              ) ;
      if ( withPort )                                {
        s += QString ( ":%1" ) . arg ( ip4 -> port ) ;
      }                                              ;
    break                                            ;
    case AF_INET6                                    :
      ipf = (quint8 *) ip6 -> IP                     ;
      for (int i=0;i<8;i++)                          {
        quint16 v                                    ;
        v   = ipf [       i * 2   ]                  ;
        v <<= 8                                      ;
        v  += ipf [ 1 + ( i * 2 ) ]                  ;
        l << QString::number((uint)v,16)             ;
      }                                              ;
      s = l . join ( ":" )                           ;
      if (withPort)                                  {
        QString r = s                                ;
        s = QString("[%1]:%2").arg(r).arg(ip4->port) ;
      }                                              ;
    break                                            ;
  }                                                  ;
  return s                                           ;
}

std::string IpAddress::toStdString(bool withPort)
{
  return toString ( withPort ) . toStdString ( ) ;
}

#if defined(Q_OS_WIN)

int IpAddress::length(void)
{
  switch ( type ( ) )                  {
    case AF_INET                       :
    return sizeof(struct sockaddr_in ) ;
    case AF_INET6                      :
    return sizeof(struct sockaddr_in6) ;
  }                                    ;
  return 0                             ;
}

#else

socklen_t IpAddress::length(void)
{
  switch ( type ( ) )                  {
    case AF_INET                       :
    return sizeof(struct sockaddr_in ) ;
    case AF_INET6                      :
    return sizeof(struct sockaddr_in6) ;
  }                                    ;
  return 0                             ;
}

#endif

bool IpAddress::obtain(struct sockaddr_in * sockin)
{
  if (size()<=0) return false                                  ;
  QString        s                                             ;
  QByteArray     IPB                                           ;
  char         * ADR                                           ;
  IPv4address  * ip4 = (IPv4address  *) data ( )               ;
  IPv6address  * ip6 = (IPv6address  *) data ( )               ;
  sockaddr_in  * d4  = (sockaddr_in  *) sockin                 ;
  sockaddr_in6 * d6  = (sockaddr_in6 *) sockin                 ;
  switch ( ip4 -> type )                                       {
    case AF_INET                                               :
      s   = QString ( "%1.%2.%3.%4"                            )
            .arg    ( ip4->IP[0]                               )
            .arg    ( ip4->IP[1]                               )
            .arg    ( ip4->IP[2]                               )
            .arg    ( ip4->IP[3]                             ) ;
      IPB = s . toUtf8 ( )                                     ;
      ADR = (char *)IPB.constData()                            ;
      ::memset(d4,0,sizeof(sockaddr_in))                       ;
      d4 -> sin_family      = AF_INET                          ;
      d4 -> sin_addr.s_addr = INADDR_ANY                       ;
      d4 -> sin_port        = htons(ip4->port)                 ;
      #if defined(Q_OS_WIN)
      d4 -> sin_addr.s_addr = ::inet_addr(ADR)                 ;
      #else
      ::inet_aton ( ADR , & ( d4 -> sin_addr ) )               ;
      #endif
    break                                                      ;
    case AF_INET6                                              :
      ::memset(d6,0,sizeof(sockaddr_in6))                      ;
      d6 -> sin6_family      = AF_INET6                        ;
      d6 -> sin6_port        = htons(ip6->port)                ;
      ::memcpy ( &d6->sin6_addr , ip6->IP , sizeof(in6_addr) ) ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}

bool IpAddress::obtain(struct sockaddr_in & sockin)
{
  return obtain ( (struct sockaddr_in *) & sockin ) ;
}

bool IpAddress::obtain(struct sockaddr_in6 & sockin)
{
  return obtain ( (struct sockaddr_in *) & sockin ) ;
}

bool IpAddress::setAddress(struct sockaddr_in & address)
{
  setType ( AF_INET )                                                   ;
  if ( AF_INET != address . sin_family) return false                    ;
  QString addr                                                          ;
  char    buff [ 128 ]                                                  ;
  char *  iaddr = NULL                                                  ;
  int     port  = 0                                                     ;
  #if defined(Q_OS_WIN)
  iaddr = (char *) InetNtop  ( AF_INET                                  ,
                               & ( address . sin_addr )                 ,
                               (PTSTR) buff                             ,
                               120                                    ) ;
  #if defined(UNICODE)
  if ( NULL != iaddr ) addr = QString::fromUtf16((const ushort *)iaddr) ;
  #else
  if ( NULL != iaddr ) addr = QString::fromUtf8(iaddr)                  ;
  #endif
  #else
  iaddr = (char *) inet_ntop ( AF_INET                                  ,
                               & ( address . sin_addr )                 ,
                               buff                                     ,
                               120                                    ) ;
  if ( NULL != iaddr ) addr = QString::fromUtf8(iaddr)                  ;
  #endif
  if ( addr . length ( ) <= 0 ) return false                            ;
  port = ntohs  ( address . sin_port    )                               ;
  addr . append ( ":"                   )                               ;
  addr . append ( QString::number(port) )                               ;
  return setAddress ( addr )                                            ;
}

bool IpAddress::setAddress(struct sockaddr_in6 & address)
{
  setType ( AF_INET6 )                                         ;
  if ( AF_INET6 != address . sin6_family) return false         ;
  IPv6address * ip6 = (IPv6address  *) data ( )                ;
  ip6->port = ntohs ( address . sin6_port )                    ;
  ::memcpy ( ip6->IP , &address.sin6_addr , sizeof(in6_addr) ) ;
  return true                                                  ;
}

bool IpAddress::setAddress(struct sockaddr * address)
{
  switch ( address -> sa_family )                                          {
    case AF_INET  : return setAddress ( *(struct sockaddr_in  *) address ) ;
    case AF_INET6 : return setAddress ( *(struct sockaddr_in6 *) address ) ;
  }                                                                        ;
  return false                                                             ;
}

bool IpAddress::setAddress(const char * address)
{
  if ( NULL ==  address        ) return false         ;
  if ( strlen ( address ) <= 0 ) return false         ;
  return setAddress ( QString::fromUtf8 ( address ) ) ;
}

bool IpAddress::setAddress(QString address)
{
  IPv4address * ip4 = NULL                                 ;
  IPv6address * ip6 = NULL                                 ;
  quint8      * ipf = NULL                                 ;
  QStringList   LL                                         ;
  QString       AA                                         ;
  setType ( AF_INET )                                      ;
  if ( address.contains(':') )                             {
    if ( address.contains('.') )                           {
      ip4 = (IPv4address *)data()                          ;
      LL  = address.split(':')                             ;
      if ( 2 != LL.count() ) return false                  ;
      AA   = LL[1]                                         ;
      ip4 -> port = AA.toInt()                             ;
      AA   = LL[0]                                         ;
      LL   = AA.split('.')                                 ;
      if ( 4 == LL.count() )                               {
        int  v    = 0                                      ;
        bool okay = false                                  ;
        for (int i=0;i<4;i++)                              {
          okay = false                                     ;
          v    = LL [ i ] . toInt ( &okay )                ;
          if ( ! okay ) return false                       ;
          ip4 -> IP [ i ] = (unsigned char) v              ;
        }                                                  ;
      } else return false                                  ;
    } else                                                 {
      setType ( AF_INET6 )                                 ;
      ip6 = (IPv6address *)data()                          ;
      if ( address . contains ( "["  )                    ||
           address . contains ( "]:" )                   ) {
        LL = address . split ( "]:" )                      ;
        if ( LL.count() != 2 ) return false                ;
        int  v    = 0                                      ;
        bool okay = false                                  ;
        v = LL [ 1 ] . toInt ( &okay )                     ;
        if ( ! okay ) return false                         ;
        ip6 -> port = v                                    ;
        AA = LL [ 0 ]                                      ;
        v  = AA.indexOf('[')                               ;
        if ( v >= 0 ) AA . remove ( 0 , v + 1 )            ;
      } else AA = address                                  ;
      //////////////////////////////////////////////////////
      if ( AA . contains ( "::" ) )                        {
        LL = AA . split ( "::" )                           ;
        if ( LL.count() != 2 ) return false                ;
        else                                               {
          int     v    = 0                                 ;
          bool    okay = false                             ;
          QString H    = LL [ 0 ]                          ;
          QString T    = LL [ 1 ]                          ;
          ipf = (quint8 *) ip6 -> IP                       ;
          for (int i=0;i<8;i++) ipf[i] = 0                 ;
          LL = H . split ( ":" )                           ;
          for (int i=0;i<LL.count();i++)                   {
            v = LL [ i ] . toInt ( &okay , 16 )            ;
            if ( ! okay ) return false                     ;
            ipf [ 1 + ( i * 2 ) ] = (quint8) ( v & 0xFF )  ;
            v                   >>= 8                      ;
            ipf [       i * 2   ] = (quint8) ( v & 0xFF )  ;
          }                                                ;
          LL = T . split ( ":" )                           ;
          for (int i=0;i<LL.count();i++)                   {
            int j = LL.count() - i - 1                     ;
            v = LL [ j ] . toInt ( &okay , 16 )            ;
            if ( ! okay ) return false                     ;
            j = 7 - i                                      ;
            ipf [ 1 + ( j * 2 ) ] = (quint8) ( v & 0xFF )  ;
            v                   >>= 8                      ;
            ipf [       j * 2   ] = (quint8) ( v & 0xFF )  ;
          }                                                ;
        }                                                  ;
      } else
      if ( AA . contains ( ":" ) )                         {
        LL = AA . split ( ':' )                            ;
        if ( LL . count ( ) != 8 ) return false            ;
        ipf = (quint8 *) ip6 -> IP                         ;
        for (int i=0;i<8;i++)                              {
          int  v    = 0                                    ;
          bool okay = false                                ;
          v = LL [ i ] . toInt ( &okay , 16 )              ;
          if ( ! okay ) return false                       ;
          ipf [ 1 + ( i * 2 ) ] = (quint8) ( v & 0xFF )    ;
          v                   >>= 8                        ;
          ipf [       i * 2   ] = (quint8) ( v & 0xFF )    ;
        }                                                  ;
        return true                                        ;
      }                                                    ;
      return false                                         ;
    }                                                      ;
  } else                                                   {
    ip4 = (IPv4address *)data()                            ;
    LL  = address.split('.')                               ;
    if ( 4 == LL.count() )                                 {
      int  v    = 0                                        ;
      bool okay = false                                    ;
      for (int i=0;i<4;i++)                                {
        okay = false                                       ;
        v    = LL [ i ] . toInt ( &okay )                  ;
        if ( ! okay ) return false                         ;
        ip4 -> IP [ i ] = (unsigned char) v                ;
      }                                                    ;
    } else return false                                    ;
  }                                                        ;
  return true                                              ;
}

bool IpAddress::setAddress(std::string address)
{
  if ( address . length ( ) <= 0 ) return false            ;
  return setAddress ( QString::fromStdString ( address ) ) ;
}

bool Lookup(QString hostname,IpAddresses & addresses,QString service)
{
  QByteArray B = hostname.toUtf8()                                           ;
  if ( B . size ( ) <= 0 ) return false                                      ;
  char * hn = (char *) B . constData ( )                                     ;
  char * sr = NULL                                                           ;
  if ( strlen ( hn ) <= 0 ) return false                                     ;
  QByteArray S                                                               ;
  if ( service.length() > 0 )                                                {
    S = service . toUtf8 ( )                                                 ;
    sr = (char *) S . constData ( )                                          ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  struct addrinfo * result = NULL                                            ;
  struct addrinfo * ptr    = NULL                                            ;
  struct addrinfo   hints                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  ::memset ( &hints , 0 , sizeof(addrinfo) )                                 ;
  hints . ai_family   = AF_UNSPEC                                            ;
  hints . ai_socktype = SOCK_STREAM                                          ;
  hints . ai_protocol = IPPROTO_TCP                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( 0 != ::getaddrinfo ( hn , sr , &hints , &result ) ) return false      ;
  ////////////////////////////////////////////////////////////////////////////
  bool correct = false                                                       ;
  for ( ptr = result ; ptr != NULL ; ptr = ptr -> ai_next )                  {
    IpAddress             IP                                                 ;
    struct sockaddr_in  * d4 = NULL                                          ;
    struct sockaddr_in6 * d6 = NULL                                          ;
    switch ( ptr -> ai_family )                                              {
      case AF_UNSPEC                                                         :
      break                                                                  ;
      case AF_INET                                                           :
        d4 = (struct sockaddr_in  *) ptr -> ai_addr                          ;
        if ( NULL != d4 )                                                    {
          IP . setAddress ( *d4 )                                            ;
          addresses << IP                                                    ;
          correct = true                                                     ;
        }                                                                    ;
      break                                                                  ;
      case AF_INET6                                                          :
        d6 = (struct sockaddr_in6 *) ptr -> ai_addr                          ;
        if ( NULL != d6 )                                                    {
          IP . setAddress ( *d6 )                                            ;
          addresses << IP                                                    ;
          correct = true                                                     ;
        }                                                                    ;
      default                                                                :
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return correct                                                             ;
}

//////////////////////////////////////////////////////////////////////////////

/*! Internal socket utilities */

#ifdef Q_OS_WIN

SOCKET GetUdpSocket (void)
{
  return ::socket ( AF_INET , SOCK_DGRAM  , IPPROTO_UDP ) ;
}

SOCKET GetTcpSocket (void)
{
  return ::socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
}

bool SetNonblock(SOCKET s)
{
  unsigned long flag = 1                                ;
  return ( 0 == ::ioctlsocket ( s , FIONBIO , &flag ) ) ;
}

bool SetReuseAddress(SOCKET s)
{
  int sflag = 1                         ;
  return ( 0 == ::setsockopt            (
                  s                     ,
                  SOL_SOCKET            ,
                  SO_REUSEADDR          ,
                  (const char *) &sflag ,
                  sizeof(int)       ) ) ;
}

int GetReceiveBuffer(SOCKET s)
{
  int       r    = 0           ;
  socklen_t opts = sizeof(int) ;
  //////////////////////////////
  opts = sizeof(int)           ;
  ::getsockopt ( s             ,
                 SOL_SOCKET    ,
                 SO_RCVBUF     ,
                 (char *) &r   ,
                 &opts       ) ;
  return r                     ;
}

int GetSendingBuffer(SOCKET s)
{
  int       r    = 0           ;
  socklen_t opts = sizeof(int) ;
  //////////////////////////////
  opts = sizeof(int)           ;
  ::getsockopt ( s             ,
                 SOL_SOCKET    ,
                 SO_SNDBUF     ,
                 (char *) &r   ,
                 &opts       ) ;
  return r                     ;
}

bool SetReceiveBuffer(SOCKET s,int bsize)
{
  int r = bsize                        ;
  int a                                ;
  a = ::setsockopt ( s                 ,
                     SOL_SOCKET        ,
                     SO_RCVBUF         ,
                     (const char *) &r ,
                     sizeof(int)     ) ;
  return ( a >= 0 )                    ;
}

bool SetSendingBuffer(SOCKET s,int bsize)
{
  int r = bsize                        ;
  int a                                ;
  a = ::setsockopt ( s                 ,
                     SOL_SOCKET        ,
                     SO_SNDBUF         ,
                     (const char *) &r ,
                     sizeof(int)     ) ;
  return ( a >= 0 )                    ;
}

#else

int GetUdpSocket (void)
{
  return ::socket ( AF_INET , SOCK_DGRAM  , IPPROTO_UDP ) ;
}

int GetTcpSocket (void)
{
//  return ::socket ( AF_INET , SOCK_STREAM , 0           ) ;
  return ::socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
}

bool SetNonblock(int s)
{
  int flag = 1                           ;
  int rr                                 ;
  #if defined(O_NONBLOCK)
  flag  = ::fcntl ( s , F_GETFL , 0    ) ;
  if ( flag < 0 ) flag = 0               ;
  flag |= O_NONBLOCK                     ;
  flag  = ::fcntl ( s , F_SETFL , flag ) ;
  rr    = ( flag >= 0 ) ? 0 : -1         ;
  #else
  rr    = ::ioctl ( s , FIOBIO , &flag ) ;
  #endif
  return  ( 0 == rr )                    ;
}

bool SetReuseAddress(int s)
{
  int sflag = 1                         ;
  return ( 0 == ::setsockopt            (
                  s                     ,
                  SOL_SOCKET            ,
                  SO_REUSEADDR          ,
                  (const char *) &sflag ,
                  sizeof(int)       ) ) ;
}

int GetReceiveBuffer(int s)
{
  int       r    = 0           ;
  socklen_t opts = sizeof(int) ;
  //////////////////////////////
  opts = sizeof(int)           ;
  ::getsockopt ( s             ,
                 SOL_SOCKET    ,
                 SO_RCVBUF     ,
                 (char *) &r   ,
                 &opts       ) ;
  return r                     ;
}

int GetSendingBuffer(int s)
{
  int       r    = 0           ;
  socklen_t opts = sizeof(int) ;
  //////////////////////////////
  opts = sizeof(int)           ;
  ::getsockopt ( s             ,
                 SOL_SOCKET    ,
                 SO_SNDBUF     ,
                 (char *) &r   ,
                 &opts       ) ;
  return r                     ;
}

bool SetReceiveBuffer(int s,int bsize)
{
  int r = bsize                        ;
  int a                                ;
  a = ::setsockopt ( s                 ,
                     SOL_SOCKET        ,
                     SO_RCVBUF         ,
                     (const char *) &r ,
                     sizeof(int)     ) ;
  return ( a >= 0 )                    ;
}

bool SetSendingBuffer(int s,int bsize)
{
  int r = bsize                        ;
  int a                                ;
  a = ::setsockopt ( s                 ,
                     SOL_SOCKET        ,
                     SO_SNDBUF         ,
                     (const char *) &r ,
                     sizeof(int)     ) ;
  return ( a >= 0 )                    ;
}

#endif

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
