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

bool Client::RemoveComm(void)
{
  if ( NULL == comm ) return false        ;
  TcpListener * tl = (TcpListener *) comm ;
  switch ( tl -> Type )                   {
    case CommNothing                      :
    return false                          ;
    case CommIPv4                         :
      comm = NULL                         ;
      delete tl                           ;
    break                                 ;
    case CommIPv6                         :
    return false                          ;
    case CommZMQ                          :
    return false                          ;
  }                                       ;
  return true                             ;
}

void Client::PrepareTcp(void)
{
  TcpListener * tl = new TcpListener ( )                     ;
  ::memset ( & ( tl -> address ) , 0 , sizeof(sockaddr_in) ) ;
  tl -> Type      = CommIPv4                                 ;
  tl -> Socket    = -1                                       ;
  tl -> addresses . clear ( )                                ;
  tl -> data      = NULL                                     ;
  comm            = tl                                       ;
}

bool Client::setCommunication (int commType)
{
  if ( executing ) return false ;
  RemoveComm ( )                ;
  switch ( commType )           {
    case CommNothing            :
    return false                ;
    case CommIPv4               :
      PrepareTcp ( )            ;
    break                       ;
    case CommIPv6               :
    return false                ;
    case CommZMQ                :
    return false                ;
  }                             ;
  return true                   ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
