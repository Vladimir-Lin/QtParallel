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

FileMethod:: FileMethod ( QString n , Methods *  m )
           : Method     (         n ,            m )
{
  Information [ "Suicide" ] = true ;
}

FileMethod::~FileMethod(void)
{
}

int FileMethod::MethodType(void) const
{
  return 1300001 ;
}

QString FileMethod::help(void)
{
  return QString ( "File method is a file transfer tool" ) ;
}

bool FileMethod::capable(int flags)
{
  return true ;
}

bool FileMethod::decapitate(void)
{
  return Information [ "Suicide" ] . toInt ( ) ;
}

bool FileMethod::execute(Value & p,Value & r)
{
  if ( ! p . isStruct ( ) ) return false                          ;
  if ( p . hasMember ( "Question" ) )                             {
    QString question = p [ "Question" ]                           ;
    if ( "System"    == question  ) return System       (     r ) ;
    if ( "Devices"   == question  ) return Devices      (     r ) ;
    if ( "File"      == question  ) return GetFileInfo  ( p , r ) ;
    if ( "Directory" == question  ) return GetDirectory ( p , r ) ;
  }                                                               ;
  if ( p . hasMember ( "Action" ) ) return FileActions  ( p , r ) ;
  return true                                                     ;
}

bool FileMethod::transmitting(Header & p,QByteArray & d)
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

bool FileMethod::receiving(Header & p,QByteArray & d)
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

bool FileMethod::System(Value & r)
{
  r [ "Answer" ] = QString ( "Yes"     ) ;
  #ifdef Q_OS_WIN
  r [ "System" ] = QString ( "Windows" ) ;
  #else
  r [ "System" ] = QString ( "Linux"   ) ;
  #endif
  return true                            ;
}

bool FileMethod::Devices(Value & r)
{
  QFileInfoList FIL = QDir::drives ( )           ;
  Value d                                        ;
  ////////////////////////////////////////////////
  r [ "Answer" ] = QString ( "Yes" )             ;
  ////////////////////////////////////////////////
  if ( FIL . size ( ) <= 0 )                     {
    r [ "Devices" ] = 0                          ;
  } else                                         {
    for (int i = 0 ; i < FIL . count ( ) ; i++ ) {
      d [ i ] = FIL [ i ] . absolutePath ( )     ;
    }                                            ;
    r [ "Devices" ] = (int) FIL . size ( )       ;
    r [ "Lists"   ] = d                          ;
  }                                              ;
  ////////////////////////////////////////////////
  return true                                    ;
}

bool FileMethod::GetFileInfo(Value & p,Value & r)
{
  if ( p . hasMember ( "File" ) )                {
    QString fn = p [ "File" ]                    ;
    if ( p . hasMember ( "Directory" ) )         {
    }                                            ;
  } else                                         {
    r [ "Answer" ] = QString ( "No" )            ;
    r [ "Error"  ] = "No filename"               ;
    r [ "Code"   ] = (int) 51001                 ;
  }                                              ;
  return true                                    ;
}

bool FileMethod::GetDirectory(Value & p,Value & r)
{
  if ( p . hasMember ( "Directory" ) )           {
  } else                                         {
  }                                              ;
  return true                                    ;
}

bool FileMethod::FileActions(Value & p,Value & r)
{
  r [ "Answer" ] = QString ( "Yes" ) ;
  return true                        ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
