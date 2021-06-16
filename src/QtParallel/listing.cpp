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

#define ListName QString("system.List")

//////////////////////////////////////////////////////////////////////////////

Listing:: Listing ( Methods *  m )
        : Method  ( ListName , m )
{
}

Listing::~Listing(void)
{
}

QString Listing::MethodName(void)
{
  return ListName ;
}

int Listing::MethodType(void) const
{
  return 1100002 ;
}

QString Listing::help(void)
{
  return QString ( "List all methods available on a server as an array of strings" ) ;
}

bool Listing::capable(int flags)
{
  if ( Transmitter == ( Transmitter & flags ) ) return false ;
  if ( Receiver    == ( Receiver    & flags ) ) return false ;
  return true                                                ;
}

bool Listing::decapitate(void)
{
  return true ;
}

bool Listing::execute(Value & p,Value & r)
{ Q_UNUSED ( p )                   ;
  if ( NULL == methods )           {
    r = false                      ;
    return false                   ;
  }                                ;
  return methods -> Listings ( r ) ;
}

//////////////////////////////////////////////////////////////////////////////

#endif

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
