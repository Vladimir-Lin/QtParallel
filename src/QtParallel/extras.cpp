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

Extras:: Extras(void)
{
  Information [ "Prepared"  ] = false     ;
  Information [ "Operation" ] = "Nothing" ;
}

Extras::~Extras(void)
{
}

bool Extras::recycle(void)
{
  if ( Reader . isOpen ( ) ) Reader . close ( ) ;
  if ( Writer . isOpen ( ) ) Writer . close ( ) ;
  Information . clear ( )                       ;
  Information [ "Prepared"  ] = false           ;
  Information [ "Operation" ] = "Nothing"       ;
  return true                                   ;
}

bool Extras::destroy(void)
{
  recycle ( ) ;
  delete this ;
  return true ;
}

void Extras::Copy(QStringList & K,Header & p)
{
  QString k                              ;
  foreach ( k , K )                      {
    if ( p . Tags . contains ( k ) )     {
      Information [ k ] = p . Tags [ k ] ;
    }                                    ;
  }                                      ;
}

bool Extras::transmitting(Header & p,QByteArray & d)
{
  bool correct = true                                                        ;
  if ( ! Information [ "Prepared" ] . toBool ( ) )                           {
    QStringList K = p . Tags . keys ( )                                      ;
    QString     k                                                            ;
    QString     s                                                            ;
    //////////////////////////////////////////////////////////////////////////
    K << "Content-Length"                                                    ;
    K << "GET"                                                               ;
    K << "Device"                                                            ;
    K << "Method"                                                            ;
    K << "Filename"                                                          ;
    K << "Directory"                                                         ;
    Copy ( K , p )                                                           ;
    //////////////////////////////////////////////////////////////////////////
    if ( Information . contains ( "Directory" )                             &&
         Information . contains ( "Filename"  )                            ) {
      QDir    D ( Information [ "Directory" ] . toString( ) )                ;
      QString F = Information [ "Filename"  ] . toString( )                  ;
      if ( D . exists ( ) )                                                  {
        F = D . absoluteFilePath ( F )                                       ;
        Reader . setFileName ( F )                                           ;
        if ( Reader . open ( QIODevice::ReadOnly ) )                         {
          qint64 fs = Reader . size ( )                                      ;
          Information [ "Submit"         ] = 0                               ;
          Information [ "Prepared"       ] = true                            ;
          Information [ "Operation"      ] = "Read"                          ;
          Information [ "Content-Length" ] = fs                              ;
          p . Tags    [ "HeaderReady"    ] = true                            ;
          p . addLength ( fs        )                                        ;
          p . addMime   ( "unknown" )                                        ;
        }                                                                    ;
      }                                                                      ;
    } else                                                                   {
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
  }                                                                          ;
  if ( ! Information [ "Prepared" ] . toBool ( ) ) return false              ;
  ////////////////////////////////////////////////////////////////////////////
  QString ops = Information [ "Operation" ] . toString ( )                   ;
  if ( "Read" == ops )                                                       {
    qint64 cl = Information [ "Content-Length" ] . toLongLong ( )            ;
    qint64 pw = Information [ "Submit"         ] . toLongLong ( )            ;
    qint64 v  = 65536                                                        ;
    qint64 w  = v                                                            ;
    if ( ( pw + v ) > cl ) w = cl - pw                                       ;
    if ( w > 0 )                                                             {
      QByteArray B ( w , 0 )                                                 ;
      w = Reader . read ( B . data ( ) , w )                                 ;
      if ( w > 0 )                                                           {
        pw                     += w                                          ;
        Information [ "Submit"] = pw                                         ;
        d . append ( B )                                                     ;
      }                                                                      ;
    }                                                                        ;
    if ( w < 0 ) correct = false                                             ; /*! Read Error */
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if (   Information [ "Prepared" ] . toBool ( ) )                           {
    qint64 cl = Information [ "Content-Length" ] . toLongLong ( )            ;
    qint64 pw = Information [ "Submit"         ] . toLongLong ( )            ;
    if ( pw >= cl )                                                          {
      if ( "Read" == ops )                                                   {
        Reader . close ( )                                                   ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return correct                                                             ;
}

bool Extras::receiving(Header & p,QByteArray & d)
{
  bool correct = true                                                        ;
  if ( ! Information [ "Prepared" ] . toBool ( ) )                           {
    QStringList K = p . Tags . keys ( )                                      ;
    QString     k                                                            ;
    QString     s                                                            ;
    //////////////////////////////////////////////////////////////////////////
    K << "Content-Length"                                                    ;
    K << "PUT"                                                               ;
    K << "Device"                                                            ;
    K << "Method"                                                            ;
    K << "Filename"                                                          ;
    K << "Directory"                                                         ;
    Copy ( K , p )                                                           ;
    //////////////////////////////////////////////////////////////////////////
    if ( Information . contains ( "Directory" )                             &&
         Information . contains ( "Filename"  )                            ) {
      QDir    D ( Information [ "Directory" ] . toString( ) )                ;
      QString F = Information [ "Filename"  ] . toString( )                  ;
      if ( D . exists ( ) )                                                  {
        F = D . absoluteFilePath ( F )                                       ;
        Writer . setFileName ( F )                                           ;
        if ( Writer . open ( QIODevice::WriteOnly ) )                        {
          Information [ "Written"   ] = 0                                    ;
          Information [ "Prepared"  ] = true                                 ;
          Information [ "Operation" ] = "Write"                              ;
        }                                                                    ;
      }                                                                      ;
    } else                                                                   {
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
  }                                                                          ;
  if ( ! Information [ "Prepared" ] . toBool ( ) ) return false              ;
  ////////////////////////////////////////////////////////////////////////////
  QString ops = Information [ "Operation" ] . toString ( )                   ;
  if ( "Write" == ops )                                                      {
    if ( d . size ( ) > 0 )                                                  {
      qint64 cl = Information [ "Content-Length" ] . toLongLong ( )          ;
      qint64 pw = Information [ "Written"        ] . toLongLong ( )          ;
      qint64 v  = d . size ( )                                               ;
      qint64 w  = v                                                          ;
      if ( ( pw + v ) > cl ) w = cl - pw                                     ;
      if ( w > 0 )                                                           {
        w = Writer . write ( d . constData ( ) , w )                         ;
        d . remove ( 0 , w )                                                 ;
        pw                      += w                                         ;
        Information [ "Written"] = pw                                        ;
      }                                                                      ;
      if ( w < 0 ) correct = false                                           ; /*! Write Error */
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if (   Information [ "Prepared" ] . toBool ( ) )                           {
    qint64 cl = Information [ "Content-Length" ] . toLongLong ( )            ;
    qint64 pw = Information [ "Written"        ] . toLongLong ( )            ;
    if ( pw >= cl )                                                          {
      if ( "Write" == ops )                                                  {
        Writer . close ( )                                                   ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return correct                                                             ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
