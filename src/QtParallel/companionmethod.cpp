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

#define FriendName "Companion"

//////////////////////////////////////////////////////////////////////////////

CompanionMethod:: CompanionMethod ( QString companion , Methods * m )
                : Companion       (                                 )
                , Method          (        FriendName ,           m )
{
  Information [ "Name"  ] = companion ;
  Information [ "Alias" ] = companion ;
}

CompanionMethod::~CompanionMethod(void)
{
}

QString CompanionMethod::MethodName(void)
{
  return FriendName ;
}

void CompanionMethod::destroyer(void)
{
  delete this ;
}

int CompanionMethod::MethodType(void) const
{
  return 1200001 ;
}

bool CompanionMethod::isMethod(QString n,Header & h)
{
  if ( n != FriendName                   ) return false ;
  if ( ! h . Tags . contains ( "Alias" ) ) return false ;
  QString hn = Information [ "Alias" ] . toString ( )   ;
  QString ht = h . Tags    [ "Alias" ] . toString ( )   ;
  return ( ht == hn )                                   ;
}

QString CompanionMethod::help(void)
{
  return QString ( "Companion [%1]" ) . arg ( Companion::name ( ) ) ;
}

bool CompanionMethod::capable(int flags)
{
  return true ;
}

bool CompanionMethod::decapitate(void)
{
  return true ;
}

bool CompanionMethod::execute(Value & p,Value & r)
{
  lock   ( )             ;
  bool c = FromXml ( p ) ;
  r      = c             ;
  unlock ( )             ;
  return c               ;
}

bool CompanionMethod::transmitting(Header & p,QByteArray & d)
{
  Extras * fmd = NULL                  ;
  if ( NULL == p . Data )              {
    fmd      = new Extras ( )          ;
    p . Data = fmd                     ;
  } else                               {
    fmd      = p . Data                ;
  }                                    ;
  return fmd -> transmitting ( p , d ) ;
}

bool CompanionMethod::receiving(Header & p,QByteArray & d)
{
  Extras * fmd = NULL               ;
  if ( NULL == p . Data )           {
    fmd      = new Extras ( )       ;
    p . Data = fmd                  ;
  } else                            {
    fmd      = p . Data             ;
  }                                 ;
  return fmd -> receiving ( p , d ) ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
