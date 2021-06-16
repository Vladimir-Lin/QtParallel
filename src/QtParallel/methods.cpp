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

Methods:: Methods ( void           )
        : mutex   ( new QMutex ( ) )
{
}

Methods::~Methods (void)
{
  clean ( )            ;
  if ( NULL == mutex ) {
    delete mutex       ;
  }                    ;
}

void Methods::clear(void)
{
  methods . clear ( ) ;
}

void Methods::clean(void)
{
  QMutexLocker locker ( mutex )                    ;
  for (int i = 0 ; i < methods . count ( ) ; i++ ) {
    Method * m = methods [ i ]                     ;
    m -> assign ( NULL )                           ;
    if ( m -> decapitate ( ) ) delete m            ;
  }                                                ;
  methods . clear ( )                              ;
}

void Methods::MethodError(int level,int code)
{
}

bool Methods::join(Method * m)
{
  QMutexLocker locker ( mutex )       ;
  int index = methods . indexOf ( m ) ;
  if ( index >= 0 ) return true       ;
  methods << m                        ;
  m -> assign ( this )                ;
  return true                         ;
}

bool Methods::remove(Method * m)
{
  QMutexLocker locker ( mutex )       ;
  int index = methods . indexOf ( m ) ;
  if ( index < 0 ) return true        ;
  methods . takeAt ( index )          ;
  m -> assign ( NULL )                ;
  return true                         ;
}

Method * Methods::find(QString name,Header & header)
{
  if ( methods . count ( ) <= 0 ) return NULL                            ;
  Method * m = NULL                                                      ;
  mutex -> lock   ( )                                                    ;
  for (int i = 0 ; ( NULL == m ) && ( i < methods . count ( ) ) ; i++ )  {
    QString mn = methods [ i ] -> name ( )                               ;
    if ( methods [ i ] -> isMethod ( name , header ) ) m = methods [ i ] ;
  }                                                                      ;
  mutex -> unlock ( )                                                    ;
  return m                                                               ;
}

Method * Methods::find(QString name)
{
  if ( methods . count ( ) <= 0 ) return NULL                           ;
  Method * m = NULL                                                     ;
  mutex -> lock   ( )                                                   ;
  for (int i = 0 ; ( NULL == m ) && ( i < methods . count ( ) ) ; i++ ) {
    QString mn = methods [ i ] -> name ( )                              ;
    if ( name == mn ) m = methods [ i ]                                 ;
  }                                                                     ;
  mutex -> unlock ( )                                                   ;
  return m                                                              ;
}

Method * Methods::find(qint64 instance)
{
  if ( methods . count ( ) <= 0 ) return NULL                           ;
  Method * m = NULL                                                     ;
  mutex -> lock   ( )                                                   ;
  for (int i = 0 ; ( NULL == m ) && ( i < methods . count ( ) ) ; i++ ) {
    if ( instance == methods [ i ] -> instance ( ) ) m = methods [ i ]  ;
  }                                                                     ;
  mutex -> unlock ( )                                                   ;
  return m                                                              ;
}

Method * Methods::findType (int T)
{
  if ( methods . count ( ) <= 0 ) return NULL                           ;
  Method * m = NULL                                                     ;
  mutex -> lock   ( )                                                   ;
  for (int i = 0 ; ( NULL == m ) && ( i < methods . count ( ) ) ; i++ ) {
    if ( T == methods [ i ] -> MethodType ( ) ) m = methods [ i ]       ;
  }                                                                     ;
  mutex -> unlock ( )                                                   ;
  return m                                                              ;
}

bool Methods::Listings(Value & r)
{
  if ( 0 >= methods . size ( ) )                   {
    r = false                                      ;
    return false                                   ;
  }                                                ;
  mutex -> lock   ( )                              ;
  r . setSize ( methods . size ( ) )               ;
  for (int i = 0 ; i < methods . count ( ) ; i++ ) {
    r [ i ] = methods [ i ] -> name ( )            ;
  }                                                ;
  mutex -> unlock ( )                              ;
  return true                                      ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
