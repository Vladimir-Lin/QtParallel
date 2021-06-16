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

Companion:: Companion ( void  )
          : self      ( false )
{
  Information [ "Name"      ] = QString    ( "" ) ;
  Information [ "Candidate" ] = true              ;
  Information [ "Offline"   ] = true              ;
  mutex                       = new QMutex (    ) ;
}

Companion:: Companion ( bool s )
          : self      (      s )
{
  Information   [ "Name"      ] = QString    ( "" )            ;
  Information   [ "Candidate" ] = false                        ;
  Information   [ "Offline"   ] = false                        ;
  mutex                         = new QMutex (    )            ;
  if ( self )                                                  {
    Information [ "Name"      ] = QHostInfo::localHostName ( ) ;
    ObtainAddresses ( )                                        ;
  }                                                            ;
}

Companion::~Companion (void)
{
  if ( NULL != mutex ) delete mutex ;
  mutex = NULL                      ;
}

QString Companion::name(void)
{
  return Information [ "Name" ] . toString ( ) ;
}

bool Companion::isSelf(void)
{
  return self ;
}

bool Companion::isOffline(void)
{
  if ( self ) return false                                  ;
  if ( ! Information . contains ( "Offline" ) ) return true ;
  return Information [ "Offline" ] . toBool ( )             ;
}

void Companion::destroyer(void)
{
  delete this ;
}

bool Companion::lock(void)
{
  if ( NULL != mutex )  {
    mutex -> lock   ( ) ;
  }                     ;
  return true           ;
}

bool Companion::unlock(void)
{
  if ( NULL != mutex )  {
    mutex -> unlock ( ) ;
  }                     ;
  return true ;
}

bool Companion::retrieve(void)
{
  return false ;
}

bool Companion::FromXml(Value & v)
{
  if ( ! v . isStruct ( ) ) return false                     ;
  QStringList K = v . Members ( )                            ;
  QString     k                                              ;
  foreach ( k , K )                                          {
    if ( "Candidate" == k )                                  {
      if ( ! Information . contains ( "Candidate" ) )        {
        Information [ k ] = v [ k ] . toVariant ( )          ;
      } else                                                 {
        Information [ k ] = false                            ;
      }                                                      ;
    } else                                                   {
      Information [ k ] = v [ k ] . toVariant ( )            ;
    }                                                        ;
  }                                                          ;
  Information [ "Updated" ] = QDateTime::currentDateTime ( ) ;
  Information [ "Offline" ] = false                          ;
  ////////////////////////////////////////////////////////////
  return true                                                ;
}

bool Companion::ToXml(Value & v)
{
  v = Information ;
  return true     ;
}

bool Companion::FromHTTP(Header & h)
{
  QStringList K = h . Tags . keys ( )                 ;
  QString     k                                       ;
  foreach ( k , K )                                   {
    if ( "Candidate" == k )                           {
      if ( ! Information . contains ( "Candidate" ) ) {
        Information [ k ] = h . Tags [ k ]            ;
      }                                               ;
    } else                                            {
      Information [ k ] = h . Tags [ k ]              ;
    }                                                 ;
  }                                                   ;
  return true                                         ;
}

bool Companion::ToHTTP(Header & h)
{
  QStringList K = Information . keys ( ) ;
  QString     k                          ;
  QString     s                          ;
  foreach ( k , K )                      {
    s = Information [ k ] . toString ( ) ;
    if ( s . length ( ) > 0 )            {
      h . addItem ( k , s )              ;
    }                                    ;
  }                                      ;
  return true                            ;
}

bool Companion::FromLine(QString & m)
{
  QStringList L = m . split ( " " )                          ;
  if ( L . count ( ) < 2               ) return false        ;
  if ( 100      != L [ 0 ] . toInt ( ) ) return false        ;
  if ( name ( ) != L [ 1 ]             )                     {
    if ( name ( ) . length ( ) <= 0 )                        {
      Information [ "Name" ] = L [ 1 ]                       ;
    } else return false                                      ;
  }                                                          ;
  L . takeFirst ( )                                          ;
  L . takeFirst ( )                                          ;
  while ( L . count ( ) > 0 )                                {
    if ( L . count ( ) > 1 )                                 {
      if ( "Candidate" == L [ 0 ] )                          {
        if ( ! Information . contains ( "Candidate" ) )      {
          Information [ L [ 0 ] ] = L [ 1 ]                  ;
        }                                                    ;
      } else                                                 {
        Information [ L [ 0 ] ] = L [ 1 ]                    ;
      }                                                      ;
      L . takeFirst ( )                                      ;
    }                                                        ;
    L   . takeFirst ( )                                      ;
  }                                                          ;
  Information [ "Updated" ] = QDateTime::currentDateTime ( ) ;
  Information [ "Offline" ] = false                          ;
  return true                                                ;
}

bool Companion::ToLine(QString & m)
{
  QStringList L                                      ;
  QStringList K                                      ;
  QString     k                                      ;
  QString     s                                      ;
  ////////////////////////////////////////////////////
  lock   ( )                                         ;
  ////////////////////////////////////////////////////
  K = Information . keys ( )                         ;
  L << "100"                                         ;
  if ( Information . contains ( "Name" ) )           {
    L << Information [ "Name" ] . toString ( )       ;
  } else                                             {
    L << "LocalHost"                                 ;
  }                                                  ;
  ////////////////////////////////////////////////////
  foreach ( k , K )                                  {
    s = Information [ k ] . toString ( )             ;
    if ( ( s . length ( ) > 0 ) && ( k != "Name" ) ) {
      L << k                                         ;
      L << s                                         ;
    }                                                ;
  }                                                  ;
  ////////////////////////////////////////////////////
  unlock ( )                                         ;
  ////////////////////////////////////////////////////
  m  = L . join ( " " )                              ;
  m += "\r\n"                                        ;
  return ( m . length ( ) > 0 )                      ;
}

int Companion::Port(void)
{
  if ( Information . contains ( "Port" ) )    {
    return Information [ "Port" ] . toInt ( ) ;
  }                                           ;
  return 0                                    ;
}

QString Companion::IpAddress(void)
{
  if ( Information . contains ( "IP" ) )       {
    return Information [ "IP" ] . toString ( ) ;
  }                                            ;
  return ""                                    ;
}

QString Companion::IpAddresses(void)
{
  if ( Information . contains ( "Addresses" ) )       {
    return Information [ "Addresses" ] . toString ( ) ;
  }                                                   ;
  return ""                                           ;
}

int Companion::setPort(int p)
{
  Information [ "Port" ] = p ;
  return p                   ;
}

void Companion::ObtainAddresses(QString hn)
{
  if ( hn . length ( ) <= 0 ) hn = name ( )                         ;
  if ( hn . length ( ) <= 0 ) return                                ;
  QStringList         L                                             ;
  QHostInfo           info                                          ;
  QList<QHostAddress> address                                       ;
  bool                assigned = false                              ;
  info    = QHostInfo::fromName ( hn )                              ;
  address = info . addresses ( )                                    ;
  if ( address . count ( ) > 0 )                                    {
    for (int i=0;i<address.count();i++)                             {
      L << address [ i ] . toString ( )                             ;
      if ( QAbstractSocket::IPv4Protocol == address[i].protocol() ) {
        if ( ! assigned )                                           {
          Information [ "IP" ] = address [ i ] . toString ( )       ;
          assigned = true                                           ;
        }                                                           ;
      }                                                             ;
    }                                                               ;
  }                                                                 ;
  Information [ "Addresses" ] = L . join ( ";" )                    ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
