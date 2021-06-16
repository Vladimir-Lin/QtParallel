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

#include "qtparallel.h"

QT_BEGIN_NAMESPACE
BEGIN_PARALLEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////////

extern void mpsleep(int msec) ;

#pragma pack(push,1)

typedef struct {
  int Type     ;
} AskPacket    ;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////

Talk:: Talk       ( void )
     : uuid       (  0   )
     , Controller ( NULL )
     , state      (  0   )
     , comm       ( NULL )
     , port       (  0   )
{
  Information [ "Cleanup"           ] = false                          ;
  Information [ "Continue"          ] = true                           ;
  Information [ "Connecting"        ] = 5000                           ;
  Information [ "Connected"         ] = false                          ;
  Information [ "ReadBuffer"        ] = 16384                          ;
  Information [ "MaxWrite"          ] = 16384                          ;
  Information [ "SendingBuffer"     ] = 65536                          ;
  Information [ "ReceiveBuffer"     ] = 65536                          ;
  Information [ "RealSendingBuffer" ] = 0                              ;
  Information [ "RealReceiveBuffer" ] = 0                              ;
  Information [ "Lastest"           ] = QDateTime::currentDateTime ( ) ;
}

Talk::~Talk (void)
{
}

bool Talk::isContinue(void)
{
  if ( NULL != Controller )                                       {
    if ( ! (*Controller) ) return false                           ;
  }                                                               ;
  if ( Information . contains ( "Continue" ) )                    {
    if ( ! Information [ "Continue" ] . toBool ( ) ) return false ;
  }                                                               ;
  return true                                                     ;
}

void Talk::destroyer(void)
{
  delete this ;
}

int & Talk::CurrentState(void)
{
  return state ;
}

bool Talk::Lookup(void)
{
  if ( NULL == comm ) return false    ;
  AskPacket * al = (AskPacket *) comm ;
  switch ( al -> Type )               {
    case CommNothing                  :
    return false                      ;
    case CommIPv4                     :
    return LookupTcp ( )              ;
    case CommIPv6                     :
    return false                      ;
    case CommZMQ                      :
    return false                      ;
  }                                   ;
  return true                         ;
}

bool Talk::ConnectTo(void)
{
  if ( NULL == comm ) return false    ;
  AskPacket * al = (AskPacket *) comm ;
  switch ( al -> Type )               {
    case CommNothing                  :
    return false                      ;
    case CommIPv4                     :
    return ConnectTcp ( )             ;
    case CommIPv6                     :
    return false                      ;
    case CommZMQ                      :
    return false                      ;
  }                                   ;
  return true                         ;
}

bool Talk::Accepted(void * packet)
{
  if ( NULL == packet ) return false    ;
  AskPacket * al = (AskPacket *) packet ;
  switch ( al -> Type )                 {
    case CommNothing                    :
      comm = NULL                       ;
    return false                        ;
    case CommIPv4                       :
      comm = packet                     ;
    break                               ;
    case CommIPv6                       :
      comm = NULL                       ;
    return false                        ;
    case CommZMQ                        :
      comm = NULL                       ;
    return false                        ;
  }                                     ;
  return true                           ;
}

qint64 Talk::IdleTime(void)
{
  QDateTime LT = Information [ "Lastest" ] . toDateTime ( ) ;
  return LT . msecsTo ( QDateTime::currentDateTime ( ) )    ;
}

bool Talk::isTimeout(qint64 msec)
{
  QDateTime LT = Information [ "Lastest" ] . toDateTime ( )         ;
  return ( LT . msecsTo ( QDateTime::currentDateTime ( ) ) > msec ) ;
}

bool Talk::isTimeout(QVariant msec)
{
  return isTimeout ( msec . toLongLong ( ) ) ;
}

bool Talk::isConnected(void)
{
  return Information [ "Connected" ] . toBool ( ) ;
}

void Talk::TeaTime(int a,int s)
{
  switch ( s )        {
    case 0            :
      mpsleep ( 150 ) ;
    break             ;
    default           :
      mpsleep (   1 ) ;
    break             ;
  }                   ;
}

QString Talk::getErrorMsg(void)
{
  return getErrorMsg ( getError ( ) ) ;
}

QString Talk::getErrorMsg(int error)
{
  return QString ( "Error %1" ) . arg ( error ) ;
}

void Talk::TalkError(int level,int code)
{
}

bool Talk::Close(void)
{
  if ( NULL == comm ) return false    ;
  AskPacket * al = (AskPacket *) comm ;
  Information [ "Connected" ] = false ;
  switch ( al -> Type )               {
    case CommNothing                  :
    return false                      ;
    case CommIPv4                     :
    return CloseTcp ( )               ;
    case CommIPv6                     :
    return false                      ;
    case CommZMQ                      :
    return false                      ;
  }                                   ;
  return false                        ;
}

int Talk::Receive(QByteArray & d,int u)
{
  if ( NULL == comm ) return -1                          ;
  AskPacket * al = (AskPacket *) comm                    ;
  int rbs = Information [ "ReadBuffer" ] . toInt ( ) + 1 ;
  if ( rbs != ReadBuffer . size ( ) )                    {
    ReadBuffer . resize ( rbs )                          ;
  }                                                      ;
  switch ( al -> Type )                                  {
    case CommNothing                                     :
    return -1                                            ;
    case CommIPv4                                        :
    return ReceiveTcp ( d , u )                          ;
    case CommIPv6                                        :
    return -1                                            ;
    case CommZMQ                                         :
    return -1                                            ;
  }                                                      ;
  return -1                                              ;
}

int Talk::Transmit(QByteArray & d,int u)
{
  if ( NULL == comm ) return -1       ;
  AskPacket * al = (AskPacket *) comm ;
  switch ( al -> Type )               {
    case CommNothing                  :
    return -1                         ;
    case CommIPv4                     :
    return TransmitTcp ( d , u )      ;
    case CommIPv6                     :
    return -1                         ;
    case CommZMQ                      :
    return -1                         ;
  }                                   ;
  return -1                           ;
}

bool Talk::PourIn(void)
{
  int rt                                                             ;
  rt = Receive ( pending , Information [ "Selecting" ] . toInt ( ) ) ;
  if ( rt >  0 )                                                     {
    Information [ "Exception" ] = 0                                  ;
  } else if ( rt == 0 )                                              {
    if ( isTimeout ( Information [ "Timeout" ] ) ) return false      ;
  } else                                                             {
    Information["Exception"] = Information["Exception"].toInt() + 1  ;
    if ( Information [ "Exception"  ] . toInt ( )                    >
         Information [ "Exceptions" ] . toInt ( )                  ) {
      return false                                                   ;
    }                                                                ;
  }                                                                  ;
  return true                                                        ;
}

bool Talk::ToPeer(void)
{
  int rt                                                            ;
  rt = Transmit ( SendBuffer                                        ,
                  Information [ "Selecting" ] . toInt ( )         ) ;
  if ( rt >  0 )                                                    {
    Information [ "Exception" ] = 0                                 ;
  } else if ( rt == 0 )                                             {
    if ( isTimeout ( Information [ "Timeout" ] ) ) return false     ;
  } else                                                            {
    Information["Exception"] = Information["Exception"].toInt() + 1 ;
    if ( Information [ "Exception"  ] . toInt ( )                   >
         Information [ "Exceptions" ] . toInt ( )                 ) {
      return false                                                  ;
    }                                                               ;
  }                                                                 ;
  return true                                                       ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
