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

ChatMethod:: ChatMethod ( QString n , Methods *  m )
           : Method     (         n ,            m )
{
  Information [ "Suicide" ] = false ;
}

ChatMethod::~ChatMethod(void)
{
}

int ChatMethod::MethodType(void) const
{
  return 1300001 ;
}

QString ChatMethod::help(void)
{
  return QString ( "Chat method is a RPC chat tool" ) ;
}

bool ChatMethod::capable(int flags)
{
  return true ;
}

bool ChatMethod::decapitate(void)
{
  return Information [ "Suicide" ] . toInt ( ) ;
}

bool ChatMethod::execute(Value & p,Value & r)
{
  if ( p . hasMember ( "Message" ) ) {
  } else                             {
  }                                  ;
  return true                        ;
}

bool ChatMethod::transmitting(Header & p,QByteArray & d)
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

bool ChatMethod::receiving(Header & p,QByteArray & d)
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
