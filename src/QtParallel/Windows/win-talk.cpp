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
  SOCKET             Socket    ;
  struct sockaddr_in address   ;
  IpAddresses        addresses ;
  void             * data      ;
} TcpListener                  ;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////

bool Talk::nonFatalError (void)
{
  int err = getError ( )               ;
  return ( ( err == WSAEINPROGRESS )  ||
           ( err == EAGAIN         )  ||
           ( err == WSAEWOULDBLOCK )  ||
           ( err == WSAEINTR       ) ) ;
}

int Talk::getError(void)
{
  return ::WSAGetLastError ( ) ;
}

bool Talk::LookupTcp(void)
{
  if ( ! Information . contains ( "Hostname" )            ) return false ;
  if ( NULL == comm                                       ) return false ;
  TcpListener * tl = (TcpListener *) comm                                ;
  if ( Information [ "Connected" ] . toBool ( ) )                        {
    if ( tl -> addresses . count ( ) > 0                  ) return true  ;
  }                                                                      ;
  QString       hn = Information [ "Hostname" ] . toString ( )           ;
  tl -> addresses . clear ( )                                            ;
  if ( ! QTPARALLEL_NAMESPACE :: Lookup(hn,tl->addresses) ) return false ;
  return ( tl -> addresses . count ( ) > 0 )                             ;
}

bool Talk::TcpWorking(qint64 msec)
{
  if ( NULL == comm ) return false                                           ;
  TcpListener             * tl      = (TcpListener *) comm                   ;
  QDateTime                 DT      = QDateTime::currentDateTime ( )         ;
  bool                      correct = false                                  ;
  bool                      expire  = false                                  ;
  int                       rt                                               ;
  struct timeval            tv                                               ;
  fd_set                    inFd                                             ;
  fd_set                    outFd                                            ;
  fd_set                    extFd                                            ;
  ////////////////////////////////////////////////////////////////////////////
  FD_ZERO (                &inFd  )                                          ;
  FD_ZERO (                &outFd )                                          ;
  FD_ZERO (                &extFd )                                          ;
  FD_SET  ( tl -> Socket , &inFd  )                                          ;
  FD_SET  ( tl -> Socket , &outFd )                                          ;
  FD_SET  ( tl -> Socket , &extFd )                                          ;
  ////////////////////////////////////////////////////////////////////////////
  tv . tv_sec  =     0                                                       ;
  tv . tv_usec = 10000                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  while ( ( ! expire ) && ( ! correct ) && isContinue ( ) )                  {
    rt = ::select ( tl -> Socket + 1 , &inFd , &outFd , &extFd , &tv )       ;
    if ( rt > 0 )                                                            {
      if ( ( ! correct ) && FD_ISSET ( tl -> Socket , &inFd  )             ) {
        correct = true                                                       ;
      }                                                                      ;
      if ( ( ! correct ) && FD_ISSET ( tl -> Socket , &outFd )             ) {
        correct = true                                                       ;
      }                                                                      ;
      if ( ( ! correct ) && FD_ISSET ( tl -> Socket , &extFd  )            ) {
        expire  = true                                                       ;
      }                                                                      ;
    }                                                                        ;
    if ( ! correct )                                                         {
      if ( DT . msecsTo ( QDateTime::currentDateTime ( ) ) > msec )          {
        expire  = true                                                       ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  return correct                                                             ;
}

bool Talk::ConnectTcp(void)
{
  if ( port <= 0                        ) return false                       ;
  if ( NULL == comm                     ) return false                       ;
  TcpListener * tl = (TcpListener *) comm                                    ;
  if ( tl -> addresses . count ( ) <= 0 ) return false                       ;
  ////////////////////////////////////////////////////////////////////////////
  int rr                                                                     ;
  if ( INVALID_SOCKET == tl -> Socket )                                      {
    /*! obtain a socket */
    tl -> Socket = GetTcpSocket ( )                                          ;
    if ( INVALID_SOCKET == tl -> Socket )                                    {
      TalkError     ( 60 , 10001   )                                         ;
      return false                                                           ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    /*! set non-block socket */
    if ( ! SetNonblock ( tl -> Socket ) )                                    {
      TalkError     ( 60 , 10002   )                                         ;
      ::closesocket ( tl -> Socket )                                         ;
      return false                                                           ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SetSendingBuffer ( tl -> Socket , Information["SendingBuffer" ].toInt()) ;
    SetReceiveBuffer ( tl -> Socket , Information["ReceiveBuffer" ].toInt()) ;
    Information [ "RealSendingBuffer" ] = GetSendingBuffer ( tl -> Socket  ) ;
    Information [ "RealReceiveBuffer" ] = GetReceiveBuffer ( tl -> Socket  ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  bool    correct    = false                                                 ;
  qint64  connecting = Information [ "Connecting" ] . toLongLong ( )         ;
  QString ADDRS                                                              ;
  int     result                                                             ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0                                                             ;
       ( ! correct )                                                        &&
       isContinue ( )                                                       &&
       ( i < tl -> addresses . count ( ) )                                   ;
       i++                                                                 ) {
    if ( AF_INET == tl -> addresses [ i ] . type ( ) )                       {
      ADDRS = tl -> addresses [ i ] . toString ( false )                     ;
      if ( ADDRS . length ( ) > 0 )                                          {
        struct sockaddr_in saddr                                             ;
        tl -> addresses [ i ] . setPort ( port  )                            ;
        if ( tl -> addresses [ i ] . obtain ( saddr ) )                      {
          /*! connect and test if it is really connected */
          result  = ::connect ( tl -> Socket                                 ,
                                (struct sockaddr *)&saddr                    ,
                                sizeof(sockaddr_in)                        ) ;
          correct = ( 0 == result ) || nonFatalError ( )                     ;
          if ( correct ) correct = TcpWorking ( connecting )                 ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  Information [ "Connected" ] = correct                                      ;
  return correct                                                             ;
}

bool Talk::CloseTcp(void)
{
  if ( NULL == comm     ) return false               ;
  TcpListener * tl = (TcpListener *) comm            ;
  if ( INVALID_SOCKET == tl -> Socket ) return false ;
  ////////////////////////////////////////////////////
  bool cc = ( 0 == ::closesocket ( tl -> Socket ) )  ;
  if ( cc ) tl -> Socket = INVALID_SOCKET            ;
  return cc                                          ;
}

int Talk::ReceiveTcp(QByteArray & d,int u)
{
  if ( NULL == comm     ) return -1                                          ;
  TcpListener * tl = (TcpListener *) comm                                    ;
  if ( INVALID_SOCKET == tl -> Socket ) return -1                            ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Information [ "ReceiveBuffer"     ] . toInt ( )                      !=
       Information [ "RealReceiveBuffer" ] . toInt ( )                     ) {
    SetReceiveBuffer ( tl -> Socket , Information["ReceiveBuffer" ].toInt()) ;
    Information [ "RealReceiveBuffer" ] = GetReceiveBuffer ( tl -> Socket  ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  struct timeval tv                                                          ;
  fd_set         rd                                                          ;
  fd_set         ed                                                          ;
  int            rt                                                          ;
  int            len  = ReadBuffer . size ( ) - 1                            ;
  ////////////////////////////////////////////////////////////////////////////
  if ( len <= 0 ) return -1                                                  ;
  tv . tv_sec  = u / 1000000                                                 ;
  tv . tv_usec = u % 1000000                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  FD_ZERO (                &rd )                                             ;
  FD_ZERO (                &ed )                                             ;
  FD_SET  ( tl -> Socket , &rd )                                             ;
  FD_SET  ( tl -> Socket , &ed )                                             ;
  rt = ::select ( tl -> Socket + 1 , &rd , NULL , &ed , &tv )                ;
  if ( FD_ISSET ( tl -> Socket , &ed ) ) return -1                           ;
  if ( rt < 0 ) return nonFatalError ( ) ? 0 : -1                            ;
  if ( ! FD_ISSET ( tl -> Socket , &rd ) ) return 0                          ;
  ////////////////////////////////////////////////////////////////////////////
  int    n                                                                   ;
  char * buff = ReadBuffer . data ( )                                        ;
  n = ::recv ( tl -> Socket , buff , len , 0 )                               ;
  if ( n > 0 )                                                               {
    buff [ n ] = 0                                                           ;
    d . append ( buff , n )                                                  ;
    Information [ "Lastest" ] = QDateTime::currentDateTime ( )               ;
  } else if ( nonFatalError ( ) ) return 0                                   ;
  ////////////////////////////////////////////////////////////////////////////
  return n                                                                   ;
}

int Talk::TransmitTcp(QByteArray & d,int u)
{
  if ( d . size ( ) <= 0 ) return  0                                         ;
  if ( NULL == comm      ) return -1                                         ;
  TcpListener * tl = (TcpListener *) comm                                    ;
  if ( INVALID_SOCKET == tl -> Socket  ) return -1                           ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Information [ "SendingBuffer"     ] . toInt ( )                      !=
       Information [ "RealSendingBuffer" ] . toInt ( )                     ) {
    SetSendingBuffer ( tl -> Socket , Information["SendingBuffer" ].toInt()) ;
    Information [ "RealSendingBuffer" ] = GetSendingBuffer ( tl -> Socket  ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  struct timeval tv                                                          ;
  fd_set         wr                                                          ;
  fd_set         ed                                                          ;
  int            rt                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  tv . tv_sec  = u / 1000000                                                 ;
  tv . tv_usec = u % 1000000                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  FD_ZERO (                &wr )                                             ;
  FD_ZERO (                &ed )                                             ;
  FD_SET  ( tl -> Socket , &wr )                                             ;
  FD_SET  ( tl -> Socket , &ed )                                             ;
  rt = ::select ( tl -> Socket + 1 , NULL , &wr , &ed , &tv )                ;
  if ( FD_ISSET ( tl -> Socket , &ed ) ) return -1                           ;
  if ( rt < 0 ) return nonFatalError ( ) ? 0 : -1                            ;
  if ( ! FD_ISSET ( tl -> Socket , &wr ) ) return 0                          ;
  ////////////////////////////////////////////////////////////////////////////
  int    mw   = Information [ "MaxWrite" ] . toInt ( )                       ;
  int    n    = d . size ( )                                                 ;
  char * buff = d . data ( )                                                 ;
  if ( n > mw ) n = mw                                                       ;
  n = ::send ( tl -> Socket , buff , n , 0)                                  ;
  if ( n > 0 )                                                               {
    d . remove ( 0 , n )                                                     ;
    Information [ "Lastest" ] = QDateTime::currentDateTime ( )               ;
  } else if ( nonFatalError ( ) ) return 0                                   ;
  ////////////////////////////////////////////////////////////////////////////
  return n                                                                   ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
