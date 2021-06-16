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

bool Server::RemoveComm(void)
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

bool Server::PrepareTcp(void)
{
  if ( NULL == comm ) return false                                               ;
  IpAddress     ip                                                               ;
  TcpListener * tl                    = (TcpListener *) comm                     ;
  int           rbs                   = Information [ "ReadBuffer" ] . toInt ( ) ;
  state                               = ReadHeader                               ;
  header                              . clear ( )                                ;
  pending                             . clear ( )                                ;
  http                                = ""                                       ;
  request                             = ""                                       ;
  response                            = ""                                       ;
  Information [ "Lastest"           ] = QDateTime::currentDateTime ( )           ;
  ReadBuffer                          . resize           ( rbs + 1       )       ;
  SendBuffer                          . clear            (               )       ;
  ip                                  . setAddress       ( tl -> address )       ;
  Information [ "From"              ] = ip . toString    ( true          )       ;
  Information [ "RealSendingBuffer" ] = GetSendingBuffer ( tl -> Socket  )       ;
  Information [ "RealReceiveBuffer" ] = GetReceiveBuffer ( tl -> Socket  )       ;
  Information                         . remove ( "ErrorCode"   )                 ;
  Information                         . remove ( "ErrorString" )                 ;
  return true                                                                    ;
}

bool Server::WatchTcp(void)
{
  if ( NULL == comm ) return false        ;
  TcpListener * tl = (TcpListener *) comm ;
  return NormalWatch ( )                  ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
