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

typedef struct                 {
  int                Type      ;
  int                Socket    ;
  struct sockaddr_in address   ;
  IpAddresses        addresses ;
  void             * data      ;
} TcpListener                  ;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////

bool Listener::BindTcp(void)
{
  if ( NULL != Listening ) delete Listening          ;
  Listening  = NULL                                  ;
  ////////////////////////////////////////////////////
  int fd = ::socket ( AF_INET , SOCK_STREAM , 0 )    ;
  if ( 0 > fd )                                      {
    ListenerError ( 60 , 10001 )                     ;
    return false                                     ;
  }                                                  ;
  ////////////////////////////////////////////////////
  int len  = sizeof(sockaddr_in)                     ;
  int rr                                             ;
  if ( ! SetNonblock ( fd ) )                        {
    ListenerError ( 60 , 10002 )                     ;
    ::close       ( fd         )                     ;
    return false                                     ;
  }                                                  ;
  ////////////////////////////////////////////////////
  if ( ! SetReuseAddress ( fd ) )                    {
    ListenerError ( 60 , 10003 )                     ;
    ::close       ( fd         )                     ;
    return false                                     ;
  }                                                  ;
  ////////////////////////////////////////////////////
  struct sockaddr_in sa                              ;
  ::memset ( &sa , 0 , sizeof(sockaddr_in) )         ;
  sa . sin_family        = AF_INET                   ;
  sa . sin_addr . s_addr = htonl(INADDR_ANY)         ;
  sa . sin_port          = htons((u_short) tcpPort)  ;
  ////////////////////////////////////////////////////
  rr = ::bind ( fd , (struct sockaddr *) &sa , len ) ;
  if ( 0 != rr )                                     {
    ListenerError ( 60 , 10004 )                     ;
    ::close       ( fd         )                     ;
    return false                                     ;
  }                                                  ;
  ////////////////////////////////////////////////////
  rr = ::listen ( fd , backlog() )                   ;
  if ( 0 != rr )                                     {
    ListenerError ( 60 , 10005 )                     ;
    ::close       ( fd         )                     ;
    return false                                     ;
  }                                                  ;
  ////////////////////////////////////////////////////
  TcpListener * tl = new TcpListener ( )             ;
  tl -> Type   = CommIPv4                            ;
  tl -> Socket = fd                                  ;
  tl -> data   = NULL                                ;
  ::memcpy( & ( tl -> address ) , & sa, len )        ;
  ////////////////////////////////////////////////////
  Listening    = tl                                  ;
  return true                                        ;
}

bool Listener::WatchTcp(void)
{
  if ( NULL == Listening ) return false                     ;
  TcpListener * tl = (TcpListener *) Listening              ;
  if ( CommIPv4 != tl -> Type ) return false                ;
  ///////////////////////////////////////////////////////////
  int            fd         = tl -> Socket                  ;
  bool           go         = true                          ;
  bool           exceptions = false                         ;
  struct timeval tv                                         ;
  fd_set         inFd                                       ;
  fd_set         excFd                                      ;
  int            rr                                         ;
  qint64         dt                                         ;
  ///////////////////////////////////////////////////////////
  while ( go && ( ! AbortListening ( ) ) )                  {
    FD_ZERO (      &inFd  )                                 ;
    FD_ZERO (      &excFd )                                 ;
    FD_SET  ( fd , &inFd  )                                 ;
    FD_SET  ( fd , &excFd )                                 ;
    /////////////////////////////////////////////////////////
    dt  = await ( )                                         ;
    dt *= 1000                                              ;
    tv . tv_sec  = dt / 1000000                             ;
    tv . tv_usec = dt % 1000000                             ;
    rr = ::select ( fd + 1                                  ,
                    &inFd                                   ,
                    NULL                                    ,
                    &excFd                                  ,
                    &tv                                   ) ;
    if ( rr <  0 )                                          {
      go         = false                                    ;
      exceptions = true                                     ;
      ListenerError ( 60 , 20001 )                          ;
      break                                                 ;
    }                                                       ;
    if ( rr == 0 )                                          {
      go = ListenerGaps ( )                                 ;
      continue                                              ;
    }                                                       ;
    if ( FD_ISSET ( fd , &excFd ) )                         {
      go         = false                                    ;
      exceptions = true                                     ;
      ListenerError ( 60 , 20002 )                          ;
      break                                                 ;
    }                                                       ;
    if ( AbortListening ( ) ) continue                      ;
    /////////////////////////////////////////////////////////
    if ( FD_ISSET ( fd , &inFd  ) )                         {
      SOCKET             s                                  ;
      struct sockaddr_in addr                               ;
      int                slen                               ;
      slen = sizeof(sockaddr_in)                            ;
      s = ::accept ( fd                                     ,
                     (struct sockaddr*) & addr              ,
                     &slen                                ) ;
      if ( 0 > s )                                          {
        ListenerError   ( 60 , 21001 )                      ;
      } else                                                {
        if ( ! SetNonblock ( fd ) )                         {
          ListenerError ( 60 , 21002 )                      ;
          ::closesocket ( fd         )                      ;
        } else                                              {
          TcpListener * tx = new TcpListener ( )            ;
          tx -> Type   = AF_INET                            ;
          tx -> Socket = fd                                 ;
          tx -> data   = NULL                               ;
          SetSendingBuffer ( fd , ListenSendingBuffer ( ) ) ;
          SetReceiveBuffer ( fd , ListenReceiveBuffer ( ) ) ;
          ::memcpy ( & ( tx -> address ) , & addr , slen )  ;
          if ( ! Accepting ( (void *) tx ) )                {
            ListenerError ( 60 , 21003 )                    ;
            delete tx                                       ;
          }                                                 ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    go = ListenerGaps ( )                                   ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  ::close ( fd )                                            ;
  ///////////////////////////////////////////////////////////
  if ( NULL != tl ) delete tl                               ;
  Listening = NULL                                          ;
  return ( ! exceptions )                                   ;
}

bool Listener::CloseTcpListener(void)
{
  if ( NULL == Listening ) return false        ;
  TcpListener * tl = (TcpListener *) Listening ;
  if ( CommIPv4 != tl -> Type ) return false   ;
  delete tl                                    ;
  Listening = NULL                             ;
  return true                                  ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
