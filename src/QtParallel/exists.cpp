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

#ifndef NO_SYSTEM_METHODS

//////////////////////////////////////////////////////////////////////////////

#define ExistsName QString("Exists")

//////////////////////////////////////////////////////////////////////////////

Exists:: Exists ( Methods    * m )
       : Method ( ExistsName , m )
{
}

Exists::~Exists(void)
{
}

QString Exists::MethodName(void)
{
  return ExistsName ;
}

int Exists::MethodType(void) const
{
  return 1000001 ;
}

QString Exists::help(void)
{
  return QString ( "Prove me exists" ) ;
}

bool Exists::capable(int flags)
{
  if ( Transmitter == ( Transmitter & flags ) ) return false ;
  if ( Receiver    == ( Receiver    & flags ) ) return false ;
  return true                                                ;
}

bool Exists::decapitate(void)
{
  return true ;
}

bool Exists::execute(Value & p,Value & r)
{
  QString hn = QHostInfo::localHostName ( ) ;
  r . setSize ( 3 )                         ;
  r   [ 0 ] = QString ( "Yes" )             ;
  r   [ 1 ] = hn                            ;
  r   [ 2 ] = false                         ;
  if ( p . size ( ) > 0 )                   {
    QString hx = p [ 0 ]                    ;
    r [ 2 ] = (bool) ( hx == hn )           ;
  }                                         ;
  return true                               ;
}

//////////////////////////////////////////////////////////////////////////////

#endif

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
