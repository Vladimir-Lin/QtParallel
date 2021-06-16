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

#define HelpName QString("system.Help")

//////////////////////////////////////////////////////////////////////////////

Help:: Help   ( Methods *  m )
     : Method ( HelpName , m )
{
}

Help::~Help(void)
{
}

QString Help::MethodName(void)
{
  return HelpName ;
}

int Help::MethodType(void) const
{
  return 1100001 ;
}

QString Help::help(void)
{
  return QString ( "Retrieve the help string for a named method" ) ;
}

bool Help::capable(int flags)
{
  if ( Transmitter == ( Transmitter & flags ) ) return false ;
  if ( Receiver    == ( Receiver    & flags ) ) return false ;
  return true                                                ;
}

bool Help::decapitate(void)
{
  return true ;
}

bool Help::execute(Value & p,Value & r)
{
  if ( NULL == methods )                                {
    r = false                                           ;
    return false                                        ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  Method * m = NULL                                     ;
  ///////////////////////////////////////////////////////
  if ( p . isString ( ) )                               {
    m = methods -> find ( (QString) p )                 ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  if ( p . isInt ( ) )                                  {
    m = methods -> findType ( (int) p )                 ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  if ( p . isArray ( ) )                                {
    if ( p . size ( ) > 0 )                             {
      if ( p [ 0 ] . isString ( ) )                     {
        m = methods -> find ( (QString) p  [ 0 ] )      ;
      }                                                 ;
      if ( p [ 0 ] . isInt    ( ) )                     {
        m = methods -> findType ( (int) p [ 0 ]  )      ;
      }                                                 ;
    }                                                   ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  if ( NULL == m )                                      {
    r = false                                           ;
    return false                                        ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  r = m -> help ( )                                     ;
  return true                                           ;
}

//////////////////////////////////////////////////////////////////////////////

#endif

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
