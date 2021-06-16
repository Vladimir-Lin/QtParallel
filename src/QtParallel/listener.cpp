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

extern void mpsleep(int msec) ;

//////////////////////////////////////////////////////////////////////////////

Listener:: Listener  ( void )
         : Listening ( NULL )
{
}

Listener::~Listener (void)
{
}

bool Listener::AbortListening (void)
{
  return false ;
}

bool Listener::ListenerGaps (void)
{
  mpsleep ( 10 ) ;
  return true    ;
}

int Listener::await(void)
{
  return 10 ;
}

int Listener::backlog(void)
{
  return 0 ;
}

int Listener::ListenReceiveBuffer(void)
{
  return 65536 ;
}

int Listener::ListenSendingBuffer(void)
{
  return 65536 ;
}

void Listener::ListenerError(int level,int code)
{
}

bool Listener::Accepting(void * incoming)
{
  return false ;
}

bool Listener::BindListener(void)
{
  return BindTcp  ( ) ;
}

bool Listener::Incoming(void)
{
  return WatchTcp ( ) ;
}

bool Listener::Hosting(void)
{
  while ( ! AbortListening ( ) ) {
    ListenerGaps ( )             ;
    if ( BindListener ( ) )      {
      Incoming ( )               ;
    }                            ;
  }                              ;
  return CloseListener ( )       ;
}

bool Listener::CloseListener(void)
{
  return CloseTcpListener ( ) ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
