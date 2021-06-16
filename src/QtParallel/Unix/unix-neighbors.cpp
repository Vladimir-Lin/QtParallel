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

int Neighbors::BroadcastSocket (void)
{
  int s = -1                                           ;
  int o =  1                                           ;
  int r =  1                                           ;
  int a                                                ;
  //////////////////////////////////////////////////////
  while ( 0 > s )                                      {
    s = GetUdpSocket ( )                               ;
    if ( 0 > s )                                       {
      NeighborError ( 60 , 10001 )                     ;
      if ( AbortNeighbors ( UdpOperation ) ) return -1 ;
      mpsleep ( 2 )                                    ;
    }                                                  ;
  }                                                    ;
  //////////////////////////////////////////////////////
  SetSendingBuffer ( s , SendingBufferSize ( ) )       ;
  SetReceiveBuffer ( s , ReceiveBufferSize ( ) )       ;
  //////////////////////////////////////////////////////
  while ( ! SetReuseAddress ( s ) )                    {
    NeighborError ( 60 , 10003 )                       ;
    if ( AbortNeighbors ( UdpOperation ) ) return -1   ;
    mpsleep ( 2 )                                      ;
  }                                                    ;
  //////////////////////////////////////////////////////
  do                                                   {
    a = ::setsockopt ( s                               ,
                       SOL_SOCKET                      ,
                       SO_BROADCAST                    ,
                       (const char *) &o               ,
                       sizeof(int)                   ) ;
    if ( 0 > a )                                       {
      NeighborError ( 60 , 10006 )                     ;
      if ( AbortNeighbors ( UdpOperation ) ) return -1 ;
      mpsleep ( 2 )                                    ;
    }                                                  ;
  } while ( 0 > a )                                    ;
  //////////////////////////////////////////////////////
  return (int) s                                       ;
}

void Neighbors::CloseUdp(int s)
{
  if ( 0 > s ) return ;
  ::close ( s )       ;
}

void Neighbors::setNonblock(int s)
{
  int flag = 1                               ;
  int rr                                     ;
  #if defined(O_NONBLOCK)
  flag = ::fcntl ( fd , F_GETFL , 0 )        ;
  if ( flag < 0 ) flag = 0                   ;
  flag    |= O_NONBLOCK                      ;
  flag     = ::fcntl ( fd , F_SETFL , flag ) ;
  rr = ( flag >= 0 ) ? 0 : -1                ;
  #else
  rr = ::ioctl ( fd , FIOBIO , &flags)       ;
  #endif
}

bool Neighbors::uselect(int s,long long usec)
{
  struct timeval tv                        ;
  fd_set         rd                        ;
  int            rtcode                    ;
  //////////////////////////////////////////
  if ( s < 0 )                             {
    NeighborError ( 60 , 10001 )           ;
    return false                           ;
  }                                        ;
  //////////////////////////////////////////
  FD_ZERO (     &rd )                      ;
  FD_SET  ( s , &rd )                      ;
  //////////////////////////////////////////
  tv . tv_sec  = usec / 1000000            ;
  tv . tv_usec = usec % 1000000            ;
  //////////////////////////////////////////
  rtcode = ::select ( s + 1                ,
                      &rd                  ,
                      NULL                 ,
                      NULL                 ,
                      &tv                ) ;
  //////////////////////////////////////////
  if ( rtcode < 0           ) return false ;
  if ( FD_ISSET ( s , &rd ) ) return true  ;
  return false                             ;
}

void Neighbors::AnyAddress(struct sockaddr_in & a,int port)
{
  a . sin_family        = AF_INET     ;
  a . sin_port          = htons(port) ;
  a . sin_addr . s_addr = INADDR_ANY  ;
}

void Neighbors::BroadcastAddress(struct sockaddr_in & a,int port)
{
  a . sin_family        = AF_INET          ;
  a . sin_port          = htons(port)      ;
  a . sin_addr . s_addr = INADDR_BROADCAST ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
