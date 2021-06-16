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

void RegisterParallels(void)
{
  #ifndef NO_PARALLEL_NAMESPACE
  qRegisterMetaType < QtParallel::Value      > ( "QtParallel::Value"      ) ;
  qRegisterMetaType < QtParallel::Companion  > ( "QtParallel::Companion"  ) ;
  qRegisterMetaType < QtParallel::Method     > ( "QtParallel::Method"     ) ;
  qRegisterMetaType < QtParallel::Methods    > ( "QtParallel::Methods"    ) ;
  qRegisterMetaType < QtParallel::Talk       > ( "QtParallel::Talk"       ) ;
  qRegisterMetaType < QtParallel::Client     > ( "QtParallel::Client"     ) ;
  qRegisterMetaType < QtParallel::Server     > ( "QtParallel::Server"     ) ;
  qRegisterMetaType < QtParallel::Listener   > ( "QtParallel::Listener"   ) ;
  qRegisterMetaType < QtParallel::Neighbors  > ( "QtParallel::Neighbors"  ) ;
  qRegisterMetaType < QtParallel::Dispatcher > ( "QtParallel::Dispatcher" ) ;
  #else
  qRegisterMetaType <             Value      > (             "Value"      ) ;
  qRegisterMetaType <             Companion  > (             "Companion"  ) ;
  qRegisterMetaType <             Method     > (             "Method"     ) ;
  qRegisterMetaType <             Methods    > (             "Methods"    ) ;
  qRegisterMetaType <             Talk       > (             "Talk"       ) ;
  qRegisterMetaType <             Client     > (             "Client"     ) ;
  qRegisterMetaType <             Server     > (             "Server"     ) ;
  qRegisterMetaType <             Listener   > (             "Listener"   ) ;
  qRegisterMetaType <             Neighbors  > (             "Neighbors"  ) ;
  qRegisterMetaType <             Dispatcher > (             "Dispatcher" ) ;
  #endif
}

BEGIN_PARALLEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////////

void mpsleep(int msec)
{
  #if defined(Q_OS_WIN)
  ::Sleep  ( msec        ) ;
  #else
  ::usleep ( msec * 1000 ) ;
  #endif
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
