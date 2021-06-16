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

Method:: Method   ( void )
       : methods  ( NULL )
       , Name     ( ""   )
       , Instance ( 0    )
{
}

Method:: Method   (const QString name ,
                   Methods     * m    ,
                   qint64        inst )
       : Name     ( name              )
       , methods  ( m                 )
       , Instance ( inst              )
{
  if ( NULL != methods )     {
    methods -> join ( this ) ;
  }                          ;
}

Method::~Method (void)
{
  if ( NULL != methods )       {
    methods -> remove ( this ) ;
  }                            ;
  methods = NULL               ;
}

int Method::MethodType(void) const
{
  return 1000000 ;
}

bool Method::isMethod(QString n,Header & h)
{
  if ( n == Name          ) return true                  ;
  if ( n . length ( ) > 0 ) return false                 ;
  ////////////////////////////////////////////////////////
  bool byMethod   = h . Tags . contains ( "Method"   )   ;
  bool byInstance = h . Tags . contains ( "Instance" )   ;
  Method * m          = NULL                             ;
  if ( ( ! byMethod ) && ( ! byInstance ) ) return false ;
  if ( byMethod   )                                      {
    QString s = h . Tags [ "Method" ] . toString ( )     ;
    return ( s == Name )                                 ;
  }                                                      ;
  if ( byInstance )                                      {
    qint64 i = h . Tags [ "Instance" ] . toLongLong ( )  ;
    return ( Instance == i )                             ;
  }                                                      ;
  return false                                           ;
}

QString & Method::name(void)
{
  return Name ;
}

qint64 & Method::instance(void)
{
  return Instance ;
}

QString Method::help(void)
{
  return "" ;
}

bool Method::join(Methods & m)
{
  return m . join ( this ) ;
}

void Method::assign(Methods * m)
{
  methods = m ;
}

bool Method::capable(int flags)
{
  return true ;
}

bool Method::decapitate(void)
{
  return false ;
}

bool Method::execute(Value & p,Value & r)
{
  return false ;
}

bool Method::transmitting(Header & p,QByteArray & d)
{
  return false ;
}

bool Method::receiving(Header & p,QByteArray & d)
{
  return false ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
