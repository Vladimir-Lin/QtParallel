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

extern void mpsleep(int msec) ;

//////////////////////////////////////////////////////////////////////////////

Dispatcher:: Dispatcher (void)
{
  Information [ "UdpBuffer"               ] =  1024                          ;
  Information [ "UdpReceiveBuffer"        ] = 65536                          ;
  Information [ "UdpSendingBuffer"        ] = 16384                          ;
  Information [ "ListenReceiveBuffer"     ] = 65536                          ;
  Information [ "ListenSendingBuffer"     ] = 65536                          ;
  Information [ "Backlog"                 ] =     0                          ;
  Information [ "Threads"                 ] =     0                          ;
  Information [ "Outdated"                ] = 15000                          ;
  Information [ "WatchInterval"           ] =   330                          ;
  Information [ "ListenerGaps"            ] =    10                          ;
  Information [ "ProbeNeighbor[Interval]" ] =    25                          ;
  Information [ "WaitNeighbor[Interval]"  ] =    25                          ;
  Information [ "FindNeighbor[Interval]"  ] =   435                          ;
  ParseCompanion                            = NULL                           ;
  ////////////////////////////////////////////////////////////////////////////
  join ( new Exists  ( this ) )                                              ;
  #ifndef NO_SYSTEM_METHODS
  join ( new Help    ( this ) )                                              ;
  join ( new Listing ( this ) )                                              ;
  #endif
  ////////////////////////////////////////////////////////////////////////////
  ErrorString [ 10001 ] = QString ( "fail to obtaining a socket"           ) ;
  ErrorString [ 10002 ] = QString ( "fail to set a socket as non-block"    ) ;
  ErrorString [ 10003 ] = QString ( "socket can not be reused"             ) ;
  ErrorString [ 10004 ] = QString ( "bind to local address failure"        ) ;
  ErrorString [ 10005 ] = QString ( "fail to listen a socket"              ) ;
  ErrorString [ 10006 ] = QString ( "set UDP socket to broadcast failure"  ) ;
  ErrorString [ 20001 ] = QString ( "select a socket context failure"      ) ;
  ErrorString [ 20002 ] = QString ( "socket has an exception"              ) ;
  ErrorString [ 21001 ] = QString ( "accept a socket failure"              ) ;
  ErrorString [ 21002 ] = QString ( "set accepted socket to non-block failure") ;
  ErrorString [ 21003 ] = QString ( "failure to create a new connection for incoming connection") ;
  ////////////////////////////////////////////////////////////////////////////
  DispatcherLock   ( "LockThread" )                                          ;
  DispatcherUnlock ( "LockThread" )                                          ;
  DispatcherLock   ( "Companion"  )                                          ;
  DispatcherUnlock ( "Companion"  )                                          ;
  DispatcherLock   ( "Gift"       )                                          ;
  DispatcherUnlock ( "Gift"       )                                          ;
  DispatcherLock   ( "Server"     )                                          ;
  DispatcherUnlock ( "Server"     )                                          ;
  DispatcherLock   ( "Client"     )                                          ;
  DispatcherUnlock ( "Client"     )                                          ;
}

Dispatcher::~Dispatcher (void)
{
  Purge ( ) ;
}

void Dispatcher::Purge(void)
{
  QStringList LM = LocalMutexz . keys ( ) ;
  QString     lm                          ;
  foreach ( lm , LM )                     {
    delete LocalMutexz [ lm ]             ;
  }                                       ;
  LocalMutexz . clear ( )                 ;
  /////////////////////////////////////////
  if ( NULL != ParseCompanion )           {
    delete ParseCompanion                 ;
  }                                       ;
  ParseCompanion = NULL                   ;
  /////////////////////////////////////////
  if ( companions . count ( ) > 0 )       { // remove companions
  }                                       ;
  /////////////////////////////////////////
  if ( clients    . count ( ) > 0 )       { // remove clients
  }                                       ;
}

bool Dispatcher::Initialize(void)
{
  #ifdef Q_OS_WIN
  WSADATA wsa                    ;
  ::WSAStartup ( 0x0202 , &wsa ) ;
  #endif
  return true                    ;
}

bool Dispatcher::Recovery(void)
{
  #ifdef Q_OS_WIN
  ::WSACleanup ( ) ;
  #endif
  return true ;
}

bool Dispatcher::Cancellation(void)
{
  return false ;
}

void Dispatcher::setPort(int port)
{
  udpPort = port ;
  tcpPort = port ;
}

int Dispatcher::Threads(void)
{
  return Information [ "Threads" ] . toInt ( ) ;
}

void Dispatcher::NeighborError(int level,int code)
{
  ReportError ( level , code ) ;
}

void Dispatcher::MethodError(int level,int code)
{
  ReportError ( level , code ) ;
}

void Dispatcher::ListenerError(int level,int code)
{
  ReportError ( level , code ) ;
}

void Dispatcher::ReportError(int level,int code)
{
}

void Dispatcher::DispatcherLock(QString name)
{
  if ( ! LocalMutexz . contains ( name ) ) {
    LocalMutexz [ name ] = new QMutex ( )  ;
  }                                        ;
  LocalMutexz [ name ] -> lock ( )         ;
}

void Dispatcher::DispatcherUnlock(QString name)
{
  if ( ! LocalMutexz . contains ( name ) ) return ;
  LocalMutexz [ name ] -> unlock ( )              ;
}

void Dispatcher::PushThread(void)
{
  DispatcherLock   ( "LockThread" )                                     ;
  Information [ "Threads" ] = Information [ "Threads" ] . toInt ( ) + 1 ;
  DispatcherUnlock ( "LockThread" )                                     ;
}

void Dispatcher::PopThread(void)
{
  DispatcherLock   ( "LockThread" )                                     ;
  Information [ "Threads" ] = Information [ "Threads" ] . toInt ( ) - 1 ;
  DispatcherUnlock ( "LockThread" )                                     ;
}

bool Dispatcher::hasCompanion(QString name)
{
  if ( companions . count ( ) <= 0 ) return false                        ;
  bool match = false                                                     ;
  DispatcherLock   ( "Companion" )                                       ;
  for (int i = 0 ; ( ! match ) && ( i < companions . count ( ) ) ; i++ ) {
    match = ( name == companions [ i ] -> name ( ) )                     ;
  }                                                                      ;
  DispatcherUnlock ( "Companion" )                                       ;
  return match                                                           ;
}

int Dispatcher::Offline(QString name)
{
  Companion * nf = FindCompanion ( name )         ;
  if ( NULL == nf ) return companions . count ( ) ;
  int idx                                         ;
  DispatcherLock   ( "Companion" )                ;
  idx = companions . indexOf ( nf )               ;
  if ( idx >= 0 ) companions . takeAt ( idx )     ;
  deletedCompanions << nf                         ;
  DispatcherUnlock ( "Companion" )                ;
  return companions . count ( )                   ;
}

Companion * Dispatcher::FindCompanion(QString name)
{
  Companion * nf = NULL                                                       ;
  DispatcherLock   ( "Companion" )                                            ;
  if ( companions . count ( ) >  0 )                                          {
    for (int i = 0 ; ( NULL == nf ) && ( i < companions . count ( ) ) ; i++ ) {
      if ( name == companions [ i ] -> name ( ) ) nf = companions [ i ]       ;
    }                                                                         ;
  }                                                                           ;
  DispatcherUnlock ( "Companion" )                                            ;
  return nf                                                                   ;
}

int Dispatcher::Join(Companion * companion)
{
  if ( NULL == companion ) return companions . count ( )   ;
  Companion * nf = FindCompanion ( companion -> name ( ) ) ;
  if ( NULL != nf        ) return companions . count ( )   ;
  DispatcherLock   ( "Companion" )                         ;
  companions << companion                                  ;
  DispatcherUnlock ( "Companion" )                         ;
  return companions . count ( )                            ;
}

QStringList Dispatcher::Others(void)
{
  QStringList O                                       ;
  DispatcherLock   ( "Companion" )                    ;
  for (int i = 0 ; i < companions . count ( ) ; i++ ) {
    if ( ! companions [ i ] -> isSelf ( ) )           {
      O << companions [ i ] -> name ( )               ;
    }                                                 ;
  }                                                   ;
  DispatcherUnlock ( "Companion" )                    ;
  return      O                                       ;
}

bool Dispatcher::AbortNeighbors(int m)
{
  switch ( m )            {
    case ProbeNeighbor    :
    break                 ;
    case WaitNeighbor     :
    break                 ;
    case FindNeighbor     :
    break                 ;
  }                       ;
  return Cancellation ( ) ;
}

bool Dispatcher::NeighborClosed(int m)
{
  switch ( m )         {
    case ProbeNeighbor :
    break              ;
    case WaitNeighbor  :
    break              ;
    case FindNeighbor  :
    break              ;
  }                    ;
  return true          ;
}

bool Dispatcher::NeighborGaps(int m)
{
  QString   k                                                     ;
  QDateTime t                                                     ;
  qint64    dt = 500                                              ;
  /////////////////////////////////////////////////////////////////
  if ( deletedCompanions . count ( ) > 0 )                        {
    DispatcherLock   ( "Companion" )                              ;
    for (int i = 0 ; i < deletedCompanions . count ( ) ; i++ )    {
      deletedCompanions [ i ] -> destroyer ( )                    ;
    }                                                             ;
    deletedCompanions . clear ( )                                 ;
    DispatcherUnlock ( "Companion" )                              ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  switch ( m )                                                    {
    case ProbeNeighbor                                            :
      k  = "ProbeNeighbor[Lastest]"                               ;
      dt =  Information [ "ProbeNeighbor[Interval]" ] . toInt ( ) ;
    break                                                         ;
    case WaitNeighbor                                             :
      k  = "WaitNeighbor[Lastest]"                                ;
      dt =  Information [ "WaitNeighbor[Interval]"  ] . toInt ( ) ;
      dt =  50                                                    ;
    break                                                         ;
    case FindNeighbor                                             :
      k  = "FindNeighbor[Lastest]"                                ;
      dt =  Information [ "FindNeighbor[Interval]"  ] . toInt ( ) ;
      dt = 225                                                    ;
    break                                                         ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  if ( ! Information . contains ( k ) )                           {
    Information [ k ] = QDateTime::currentDateTime ( )            ;
  }                                                               ;
  t = Information [ k ] . toDateTime ( )                          ;
  while ( t . msecsTo ( QDateTime::currentDateTime ( ) ) < dt )   {
    if ( AbortNeighbors ( m ) ) return false                      ;
    mpsleep ( 5 )                                                 ;
  }                                                               ;
  Information [ k ] = QDateTime::currentDateTime ( )              ;
  return true                                                     ;
}

int Dispatcher::NeighborBufferSize (void)
{
  return Information [ "UdpBuffer" ] . toInt ( ) ;
}

int Dispatcher::ReceiveBufferSize(void)
{
  return Information [ "UdpReceiveBuffer" ] . toInt ( ) ;
}

int Dispatcher::SendingBufferSize(void)
{
  return Information [ "UdpSendingBuffer" ] . toInt ( ) ;
}

int Dispatcher::ListenReceiveBuffer(void)
{
  return Information [ "ListenReceiveBuffer" ] . toInt ( ) ;
}

int Dispatcher::ListenSendingBuffer(void)
{
  return Information [ "ListenSendingBuffer" ] . toInt ( ) ;
}

void Dispatcher::setGift(QString gift)
{
  QByteArray G = gift . toUtf8 ( ) ;
  setGift ( G )                    ;
}

void Dispatcher::setGift(QByteArray & gift)
{
  DispatcherLock   ( "Gift" ) ;
  NeighborGift = gift         ;
  DispatcherUnlock ( "Gift" ) ;
}

int Dispatcher::NoteTo(int m,QByteArray & d)
{
  switch ( m )                    {
    case ProbeNeighbor            :
      DispatcherLock   ( "Gift" ) ;
      d = NeighborGift            ;
      DispatcherUnlock ( "Gift" ) ;
    break                         ;
    case FindNeighbor             :
      DispatcherLock   ( "Gift" ) ;
      d = NeighborGift            ;
      DispatcherUnlock ( "Gift" ) ;
    break                         ;
    case WaitNeighbor             :
    return 0                      ;
  }                               ;
  return d . size ( )             ;
}

int Dispatcher::NoteFrom(int m,QByteArray & d)
{
  switch ( m )                  {
    case ProbeNeighbor          :
      Knock . append ( d )      ;
    break                       ;
    case WaitNeighbor           :
      Knock . append ( d )      ;
    break                       ;
    case FindNeighbor           :
    return 0                    ;
  }                             ;
  return HasCompanion ( Knock ) ;
}

int Dispatcher::HasCompanion(QByteArray & line)
{
  return LineCompanion ( line ) ;
}

int Dispatcher::LineCompanion(QByteArray & line)
{
  int idx                                    ;
  do                                         {
    idx = line . indexOf ( '\n' )            ;
    if ( idx >= 0 )                          {
      QByteArray L = line . left ( idx + 1 ) ;
      QString    s = QString::fromUtf8 ( L ) ;
      line . remove ( 0 , idx + 1 )          ;
      s = s . replace ( "\n" , "" )          ;
      s = s . replace ( "\r" , "" )          ;
      if ( s . length ( ) > 0 )              {
        CompanionMessage ( s )               ;
      }                                      ;
    }                                        ;
  } while ( idx >= 0 )                       ;
  return line . size ( )                     ;
}

void Dispatcher::CompanionMessage(QString & message)
{
  if ( message . length ( ) <= 0 ) return                ;
  if ( NULL == ParseCompanion )                          {
    ParseCompanion = NewCompanion ( )                    ;
  }                                                      ;
  ParseCompanion -> Information . clear ( )              ;
  if ( ! ParseCompanion -> FromLine ( message ) ) return ;
  QString     name = ParseCompanion -> name ( )          ;
  Companion * nf   = NULL                                ;
  bool        nb   = false                               ;
  if ( hasCompanion ( name ) )                           {
    nf = FindCompanion ( name )                          ;
    if ( ( NULL != nf ) && ( ! nf -> isSelf ( ) ) )      {
      nf -> lock     (         )                         ;
      nf -> FromLine ( message )                         ;
      nf -> unlock   (         )                         ;
    }                                                    ;
  } else                                                 {
    nf  = NewCompanion   ( name    )                     ;
    nf -> lock           (         )                     ;
    nb  = nf -> FromLine ( message )                     ;
    nf -> unlock         (         )                     ;
    if ( nb )                                            {
      Join ( nf )                                        ;
    } else                                               {
      nf -> destroyer ( )                                ;
      nf  = NULL                                         ;
    }                                                    ;
  }                                                      ;
}

bool Dispatcher::DetectMyself(void)
{
  return true ;
}

Companion * Dispatcher::NewCompanion(void)
{
  return new Companion ( ) ;
}

Companion * Dispatcher::NewCompanion(QString n)
{
  return new CompanionMethod ( n , this ) ;
}

void Dispatcher::DetectByUdp(void)
{
  PushThread    ( ) ;
  WaitNeighbors ( ) ;
  PopThread     ( ) ;
}

void Dispatcher::ProbeByUdp(void)
{
  PushThread    ( ) ;
  FindNeighbors ( ) ;
  PopThread     ( ) ;
}

void Dispatcher::WatchCompanions(void)
{
  QDateTime LT     = QDateTime::currentDateTime ( )                     ;
  int       dt                                                          ;
  QString   myname = ""                                                 ;
  bool      drop   = false                                              ;
  PushThread ( )                                                        ;
  while ( ! Cancellation (  ) )                                         {
    dt = Information [ "WatchInterval" ] . toInt ( )                    ;
    if ( dt > LT . msecsTo ( QDateTime::currentDateTime ( ) ) )         {
      mpsleep ( 5 )                                                     ;
      continue                                                          ;
    }                                                                   ;
    DispatcherLock   ( "Companion" )                                    ;
    for (int i = 0 ; i < companions . count ( ) ; i++ )                 {
      Companion * nf = companions [ i ]                                 ;
      if ( nf -> isSelf ( ) )                                           {
        QString gift                                                    ;
        nf -> retrieve (      )                                         ;
        nf -> ToLine   ( gift )                                         ;
        setGift        ( gift )                                         ;
        myname = nf -> name ( )                                         ;
      } else                                                            {
        nf -> lock   ( )                                                ;
        if ( nf -> Information . contains ( "Updated" ) )               {
          QDateTime du                                                  ;
          qint64    dx                                                  ;
          dx =       Information [ "Outdated" ] . toLongLong ( )        ;
          du = nf -> Information [ "Updated"  ] . toDateTime ( )        ;
          if ( du . msecsTo ( QDateTime::currentDateTime ( ) ) > dx )   {
            nf -> Information [ "Offline" ] = true                      ;
          }                                                             ;
        } else                                                          {
          nf -> Information [ "Offline" ] = true                        ;
        }                                                               ;
        drop = nf -> isOffline ( )                                      ;
        nf -> unlock ( )                                                ;
        /////////////////////////////////////////////////////////////////
        if ( ! drop )                                                   {
          QString Z = nf -> name ( )                                    ;
          QString K = QString ( "Online[%1]" ) . arg ( Z )              ;
          if ( ! Information . contains ( K ) )                         {
            Information [ K ] = false                                   ;
          }                                                             ;
          if ( ! Information [ K ] . toBool ( ) )                       {
            if ( myname . length ( ) > 0 )                              {
              Information [ K ] = NotifyNeighbor ( myname , Z )         ; /*! Wake up CompanionMethod emitter */
            }                                                           ;
          }                                                             ;
        }                                                               ;
      }                                                                 ;
    }                                                                   ;
    DispatcherUnlock ( "Companion" )                                    ;
    LT = QDateTime::currentDateTime ( )                                 ;
  }                                                                     ;
  PopThread  ( )                                                        ;
}

bool Dispatcher::AbortListening (void)
{
  return Cancellation ( ) ;
}

bool Dispatcher::ListenerGaps (void)
{
  if ( deletedServers . count ( ) > 0 )                     {
    DispatcherLock   ( "Server" )                           ;
    for (int i = 0 ; i < deletedServers . count ( ) ; i++ ) {
      deletedServers [ i ] -> destroyer ( )                 ;
    }                                                       ;
    deletedServers . clear ( )                              ;
    DispatcherUnlock ( "Server" )                           ;
  }                                                         ;
  if ( Cancellation ( ) ) return false                      ;
  mpsleep ( Information [ "ListenerGaps" ] . toInt ( ) )    ;
  return true                                               ;
}

int Dispatcher::await(void)
{
  return Information [ "Backlog" ] . toInt ( ) ;
}

int Dispatcher::backlog(void)
{
  return 0 ;
}

bool Dispatcher::Accepting(void * incoming)
{
  if ( NULL == incoming ) return false    ;
  Server * server = NewServer ( )         ;
  if ( NULL == server   ) return false    ;
  if ( ! server -> Accept ( incoming ) )  {
    return false                          ;
  }                                       ;
  server -> setMethods ( this           ) ;
  JoinServer           ( server         ) ;
  return StartServer   ( server -> uuid ) ;
}

Server * Dispatcher::NewServer(void)
{
  return new Server ( ) ;
}

Server * Dispatcher::FindServer(SUID u)
{
  Server * s = NULL                                                       ;
  DispatcherLock   ( "Server" )                                           ;
  if ( servers . count ( ) > 0 )                                          {
    for (int i = 0 ; ( NULL == s ) && ( i < servers . count ( ) ) ; i++ ) {
      if ( u == servers [ i ] -> uuid ) s = servers [ i ]                 ;
    }                                                                     ;
  }                                                                       ;
  DispatcherUnlock ( "Server" )                                           ;
  return s                                                                ;
}

int Dispatcher::JoinServer(Server * s)
{
  UUIDs U                                          ;
  DispatcherLock   ( "Server" )                    ;
  for (int i = 0 ; i < servers . count ( ) ; i++ ) { /*! listing all current server uuid */
    U << servers [ i ] -> uuid                     ;
  }                                                ;
  s -> uuid = UniqueUuid ( U )                     ; /*! obtain an unique uuid for new server */
  servers << s                                     ; /*! join server lists */
  DispatcherUnlock ( "Server" )                    ;
  return servers . count ( )                       ;
}

int Dispatcher::RemoveServer(Server * s)
{
  if ( NULL == s ) return servers . count ( ) ;
  int idx = -1                                ;
  DispatcherLock   ( "Server" )               ;
  idx = servers . indexOf ( s )               ;
  if ( idx >= 0 ) servers . takeAt ( idx )    ;
  deletedServers << s                         ;
  DispatcherUnlock ( "Server" )               ;
  return servers . count ( )                  ;
}

bool Dispatcher::StartServer(SUID u)
{
  // VarArgs args           ;
  // args << u              ;
  // start ( RunID , args ) ;
  return false ;
}

bool Dispatcher::RunServer(SUID u)
{
  Server * s = FindServer ( u ) ;
  if ( NULL == s ) return false ;
  PushThread         (   )      ;
  ///////////////////////////////
  if ( s -> Prepare  (   )    ) {
    s    -> Watching (   )      ;
  }                             ;
  RemoveServer       ( s )      ;
  ///////////////////////////////
  PopThread          (   )      ;
  return true                   ;
}

bool Dispatcher::NotifyNeighbor(QString myself,QString neighbor)
{
  return false ;
}

bool Dispatcher::Notify(QString myself,QString neighbor,QVariantList args)
{
  QString P = QString ( "Probe[%1]"  ) . arg ( neighbor )               ;
  if ( Information . contains ( P ) )                                   {
    if ( Information [ P ] . toBool ( ) ) return true                   ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  if ( ! hasCompanion ( myself   ) ) return false                       ;
  if ( ! hasCompanion ( neighbor ) ) return false                       ;
  QtParallel::Companion * m = FindCompanion ( myself   )                ;
  QtParallel::Companion * c = FindCompanion ( neighbor )                ;
  if ( NULL == m                              ) return false            ;
  if ( NULL == c                              ) return false            ;
  if ( ! c -> Information . contains ( "IP" ) ) return false            ;
  ///////////////////////////////////////////////////////////////////////
  c -> lock   ( )                                                       ;
  ///////////////////////////////////////////////////////////////////////
  QString     K        = QString ( "Online[%1]" ) . arg ( neighbor )    ;
  QString     adss     = c -> IpAddresses ( )                           ;
  QString     ip       = c -> IpAddress   ( )                           ;
  int         Port     = c -> Port        ( )                           ;
  int         interval = 747                                            ;
  QString     tk       = QString ( "TalkTo[%1]" ) . arg ( neighbor )    ;
  ///////////////////////////////////////////////////////////////////////
  c -> unlock ( )                                                       ;
  ///////////////////////////////////////////////////////////////////////
  if ( args . count ( ) > 2 )                                           {
    interval = args [ 2 ] . toInt ( )                                   ;
    Information [ tk ] = interval                                       ;
  }                                                                     ;
  if ( ! Information . contains ( tk ) ) Information [ tk ] = interval  ;
  ///////////////////////////////////////////////////////////////////////
  if ( adss . length ( ) <= 0 ) return false                            ;
  if ( ip . length   ( ) <= 0 ) return false                            ;
  if ( Port              <= 0 ) return false                            ;
  ///////////////////////////////////////////////////////////////////////
  PushThread (          )                                               ;
  ///////////////////////////////////////////////////////////////////////
  Information [ P ] = true                                              ;
  ///////////////////////////////////////////////////////////////////////
  if ( exists ( ip , neighbor , Port ) )                                {
    Client    client ( ip , Port )                                      ;
    QDateTime LT = QDateTime::currentDateTime ( )                       ;
    bool      go = true                                                 ;
    Information [ K ] = true                                            ;
    client . Information [ "Keep-Alive" ] = 1                           ;
    client . Information [ "Cleanup"    ] = true                        ;
    client . Information [ "Alias"      ] = myself                      ;
    while ( go && ( ! Cancellation ( ) ) )                              {
      interval = Information [ tk ] . toInt ( )                         ;
      if ( interval > LT . msecsTo ( QDateTime::currentDateTime ( ) ) ) {
        mpsleep ( 5 )                                                   ;
        continue                                                        ;
      }                                                                 ;
      Value v                                                           ;
      Value r                                                           ;
      v . clear   (   )                                                 ;
      r . clear   (   )                                                 ;
      m -> lock   (   )                                                 ;
      m -> ToXml  ( v )                                                 ;
      m -> unlock (   )                                                 ;
      if ( client . execute ( CompanionMethod::MethodName() , v , r ) ) {
        Information [ K ] = true                                        ;
      } else                                                            {
        go = false                                                      ;
      }                                                                 ;
      LT = QDateTime::currentDateTime ( )                               ;
    }                                                                   ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  Information [ K ] = false                                             ;
  Information [ P ] = false                                             ;
  Offline    ( neighbor )                                               ;
  ///////////////////////////////////////////////////////////////////////
  PopThread  (          )                                               ;
  ///////////////////////////////////////////////////////////////////////
  return true                                                           ;
}

void Dispatcher::RunListener (void)
{
  PushThread ( ) ;
  Hosting    ( ) ;
  PopThread  ( ) ;
}

void Dispatcher::WaitThreads(void)
{
  while ( Threads ( ) > 0 ) {
    mpsleep ( 100 )         ;
  }                         ;
}

bool Dispatcher::exists(void)
{
  if ( tcpPort <= 0 ) return false                        ;
  Client client ( "localhost" , tcpPort )                 ;
  Value  v                                                ;
  Value  r                                                ;
  bool   c                                                ;
  v [ 0 ] = QHostInfo::localHostName ( )                  ;
  c = client . execute ( Exists::MethodName ( ) , v , r ) ;
  if ( ! c              ) return false                    ;
  if ( r . size ( ) < 3 ) return false                    ;
  QString answer = QString ( r [ 0 ] )                    ;
  if ( "Yes" != answer ) return false                     ;
  return (bool) r [ 2 ]                                   ;
}

bool Dispatcher::exists(QString ip,QString host,int p)
{
  if ( p <= 0 ) p = tcpPort                               ;
  if ( p <= 0 ) return false                              ;
  Client client ( ip , p )                                ;
  Value  v                                                ;
  Value  r                                                ;
  bool   c                                                ;
  v [ 0 ] = host                                          ;
  c = client . execute ( Exists::MethodName ( ) , v , r ) ;
  if ( ! c              ) return false                    ;
  if ( r . size ( ) < 3 ) return false                    ;
  QString answer = r [ 0 ]                                ;
  return ( "Yes" == answer )                              ;
}

SUID Dispatcher::AddClient(Client * c)
{
  SUID u = 0                            ;
  if ( NULL == c ) return u             ;
  DispatcherLock   ( "Client" )         ;
  if ( clients . indexOf ( c ) < 0 )    {
    UUIDs U                             ;
    for (int i=0;i<clients.count();i++) {
      U << clients [ i ] -> uuid        ;
    }                                   ;
    u = UniqueUuid ( U )                ;
    c -> uuid = u                       ;
    clients << c                        ;
  }                                     ;
  DispatcherUnlock ( "Client" )         ;
  return u                              ;
}

Client * Dispatcher::FindClient(SUID u)
{
  Client * c = NULL                                                       ;
  DispatcherLock   ( "Client" )                                           ;
  if ( clients . count ( ) > 0 )                                          {
    for (int i = 0 ; ( NULL == c ) && ( i < clients . count ( ) ) ; i++ ) {
      if ( u == clients [ i ] -> uuid ) c = clients [ i ]                 ;
    }                                                                     ;
  }                                                                       ;
  DispatcherUnlock ( "Client" )                                           ;
  return c                                                                ;
}

bool Dispatcher::ExecuteClient(SUID u)
{
  Client * c = FindClient ( u )                          ;
  if ( NULL == c ) return false                          ;
  bool r = c -> execute ( )                              ;
  if ( c -> Information . contains ( "Suicide" ) )       {
    bool s = c -> Information [ "Suicide" ] . toBool ( ) ;
    if ( s )                                             {
      RemoveClient ( u )                                 ;
      delete c                                           ;
    }                                                    ;
  }                                                      ;
  return r                                               ;
}

bool Dispatcher::RemoveClient(SUID u)
{
  Client * c = FindClient ( u )            ;
  if ( NULL == c ) return false            ;
  DispatcherLock   ( "Client" )            ;
  int idx = clients . indexOf ( c )        ;
  if ( idx >= 0 ) clients . takeAt ( idx ) ;
  DispatcherUnlock ( "Client" )            ;
  return ( idx >= 0 )                      ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
