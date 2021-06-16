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

Header:: Header (void)
       : Data   (NULL)
{
}

Header:: Header ( QByteArray & h )
       : Data   ( NULL           )
{
  parse ( h ) ;
}

Header:: Header (const Header & h)
       : Data   ( NULL           )
{
  Tags  = h . Tags  ;
  Items = h . Items ;
}

Header::~Header (void)
{
  clear (   ) ;
}

Header & Header::operator = (const Header & h)
{
  Tags  = h . Tags  ;
  Items = h . Items ;
  return (*this)    ;
}

Header & Header::operator = (Header & h)
{
  Tags  = h . Tags  ;
  Items = h . Items ;
  return (*this)    ;
}

void Header::clear(void)
{
  if ( NULL != Data )   {
    Data -> destroy ( ) ;
  }                     ;
  Data  = NULL          ;
  Tags  . clear ( )     ;
  Items . clear ( )     ;
}

QString Header::compose(QString t,QString v)
{
  return QString ( "%1: %2" ) . arg ( t ) . arg ( v ) ;
}

int Header::addItem(QString t,QString v)
{
  Items << compose ( t , v ) ;
  return Items . count ( )   ;
}

void Header::addLength(qint64 L)
{
  addItem ( "Content-Length" , QString::number ( L ) ) ;
  Tags    [ "Content-Length" ] =                 L     ;
}

void Header::addMime(QString mime)
{
  addItem ( "Content-Type"   , mime ) ;
  Tags    [ "Content-Type" ] = mime   ;
}

QString Header::httpHeader (void)
{
  QString L = Items . join ( "\r\n" )     ;
  while ( ! L . contains ( "\r\n\r\n" ) ) {
    L += "\r\n"                           ;
  }                                       ;
  return  L                               ;
}

bool Header::addTag(QString & tag)
{
  if ( tag . length   (         ) <= 0 ) return false ;
  int index                                           ;
  if ( tag . contains ( "HTTP/" )      )              {
    index = tag . indexOf ( "HTTP/" )                 ;
    if ( index >= 0 )                                 {
      QString el = tag                                ;
      el . remove ( 0 , index )                       ;
      index = el . indexOf ( " " )                    ;
      if ( index > 0 )                                {
        QString     et = el                           ;
        QStringList E                                 ;
        et . remove   ( 0 , index + 1 )               ;
        el = el . mid ( 0 , index     )               ;
        while ( et . at ( 0 ) . isSpace ( ) )         {
          et . remove (  0 , 1 )                      ;
        }                                             ;
        E  = et . split ( " " )                       ;
        if ( E . count ( ) > 0 )                      {
          Tags [ "RTCODE" ] = E [ 0 ]                 ;
        }                                             ;
      }                                               ;
      QStringList X = el . split ( "/" )              ;
      if ( X . count ( ) > 1 )                        {
        Tags [ X [ 0 ] ] = X [ 1 ]                    ;
      }                                               ;
    }                                                 ;
    index = tag . indexOf ( "POST"  )                 ;
    if ( index >= 0 )                                 {
      QStringList L = tag . split ( " " )             ;
      if ( L . count ( ) > 1 )                        {
        Tags [ L [ 0 ] ] = L [ 1 ]                    ;
      }                                               ;
    }                                                 ;
    index = tag . indexOf ( "GET"  )                  ;
    if ( index >= 0 )                                 {
      QStringList L = tag . split ( " " )             ;
      if ( L . count ( ) > 1 )                        {
        Tags [ L [ 0 ] ] = L [ 1 ]                    ;
      }                                               ;
    }                                                 ;
    index = tag . indexOf ( "PUT"  )                  ;
    if ( index >= 0 )                                 {
      QStringList L = tag . split ( " " )             ;
      if ( L . count ( ) > 1 )                        {
        Tags [ L [ 0 ] ] = L [ 1 ]                    ;
      }                                               ;
    }                                                 ;
    index = tag . indexOf ( "HEAD"  )                 ;
    if ( index >= 0 )                                 {
      QStringList L = tag . split ( " " )             ;
      if ( L . count ( ) > 1 )                        {
        Tags [ L [ 0 ] ] = L [ 1 ]                    ;
      }                                               ;
    }                                                 ;
    return true                                       ;
  }                                                   ;
  if ( tag . contains ( ": "    )      )              {
    index = tag . indexOf ( ": " )                    ;
    if ( index < 0 ) return false                     ;
    QString t = tag . left ( index )                  ;
    QString v = tag                                   ;
    v . remove ( 0 , index + 2 )                      ;
    Tags [ t ] = v                                    ;
    return true                                       ;
  }                                                   ;
  return   false                                      ;
}

bool Header::parse(QByteArray & h)
{
  int index = splitter ( h )              ;
  Tags . clear ( )                        ;
  if ( index < 0 ) return false           ;
  QByteArray  M = h . mid ( 0 , index )   ;
  QString     S = QString::fromUtf8 ( M ) ;
  QStringList L = S . split ( "\n" )      ;
  foreach ( S , L )                       {
    S = S . replace ( "\r" , "" )         ;
    S = S . replace ( "\n" , "" )         ;
    addTag          ( S         )         ;
  }                                       ;
  return true                             ;
}

int Header::splitter(QByteArray & header)
{
  int index                               ;
  index = header . indexOf ( "\r\n\r\n" ) ;
  if ( index >= 0 ) return index          ;
  index = header . indexOf ( "\n\n"     ) ;
  if ( index >= 0 ) return index          ;
  return -1                               ;
}

int Header::content(QByteArray & header)
{
  int index                               ;
  index = header . indexOf ( "\r\n\r\n" ) ;
  if ( index >= 0 ) return index + 4      ;
  index = header . indexOf ( "\n\n"     ) ;
  if ( index >= 0 ) return index + 2      ;
  return -1                               ;
}

bool Header::isInvoke(QString invoke)
{
  if ( ! Tags . contains ( "Instance" ) ) return false ;
  if ( ! Tags . contains ( "Invoke"   ) ) return false ;
  QString m = Tags [ "Invoke" ] . toString ( )         ;
  return ( invoke == m )                               ;
}

bool Header::isMime(QString mime)
{
  if ( ! Tags . contains ( "Content-Type" ) ) return false ;
  QString m = Tags [ "Content-Type" ] . toString ( )       ;
  return ( mime == m )                                     ;
}

qint64 Header::Length(void)
{
  if ( ! Tags . contains ( "Content-Length" ) ) return -1 ;
  return Tags [ "Content-Length" ] . toLongLong ( )       ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
