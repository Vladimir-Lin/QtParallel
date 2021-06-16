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

char XMLRPC                 [ ] = "CIOS"                      ;
char REQUEST_BEGIN          [ ] = "<?xml version=\"1.0\"?>\r\n"
                                  "<methodCall>"
                                  "<methodName>"              ;
char REQUEST_END_METHODNAME [ ] = "</methodName>\r\n"         ;
char PARAMS_TAG             [ ] = "<params>"                  ;
char PARAMS_ETAG            [ ] = "</params>"                 ;
char PARAM_TAG              [ ] = "<param>"                   ;
char PARAM_ETAG             [ ] =  "</param>"                 ;
char REQUEST_END            [ ] = "</methodCall>\r\n"         ;
char METHODRESPONSE_TAG     [ ] = "<methodResponse>"          ;
char METHODRESPONSE_ETAG    [ ] = "</methodResponse>"         ;
char FAULT_TAG              [ ] = "<fault>"                   ;

//////////////////////////////////////////////////////////////////////////////

class PreventReEntry
{
  public:

     PreventReEntry ( bool & flag ) : _flag(flag) {                }
    ~PreventReEntry (             )               { _flag = false; }

  protected:

    bool & _flag                                                   ;

} ;

//////////////////////////////////////////////////////////////////////////////

Client:: Client (void)
       : Talk   (    )
{
  Information [ "Keep-Alive"  ] =     0        ;
  Information [ "Selecting"   ] = 25000        ;
  Information [ "Exception"   ] =     0        ;
  Information [ "Exceptions"  ] =    10        ;
  Information [ "Timeout"     ] = 60000        ;
  Information [ "AcceptFault" ] = false        ;
  Information [ "URI"         ] = "/RPC2"      ;
  state                         = NoConnection ;
  executing                     = false        ;
  ExternalData                  = NULL         ;
  ExternalIO                    = NULL         ;
  setCommunication ( CommIPv4 )                ;
}

Client:: Client ( int T )
       : Talk   (       )
{
  Information [ "Keep-Alive"  ] =     0        ;
  Information [ "Selecting"   ] = 25000        ;
  Information [ "Exception"   ] =     0        ;
  Information [ "Exceptions"  ] =    10        ;
  Information [ "Timeout"     ] = 60000        ;
  Information [ "AcceptFault" ] = false        ;
  Information [ "URI"         ] = "/RPC2"      ;
  state                         = NoConnection ;
  executing                     = false        ;
  ExternalData                  = NULL         ;
  ExternalIO                    = NULL         ;
  setCommunication ( T )                       ;
}

Client:: Client ( const char * host ,
                  int          p    ,
                  const char * uri  ,
                  int          T    )
       : Talk   (                   )
{
  Information [ "Keep-Alive"  ] =     0                      ;
  Information [ "Selecting"   ] = 25000                      ;
  Information [ "Exception"   ] =     0                      ;
  Information [ "Exceptions"  ] =    10                      ;
  Information [ "Timeout"     ] = 60000                      ;
  Information [ "AcceptFault" ] = false                      ;
  Information [ "URI"         ] = "/RPC2"                    ;
  state                         = NoConnection               ;
  executing                     = false                      ;
  port                          = p                          ;
  ExternalData                  = NULL                       ;
  ExternalIO                    = NULL                       ;
  if ( NULL != uri  )                                        {
    Information [ "URI"       ] = QString::fromUtf8 ( uri  ) ;
  }                                                          ;
  if ( NULL != host )                                        {
    Information [ "Hostname"  ] = QString::fromUtf8 ( host ) ;
  }                                                          ;
  setCommunication ( T )                                     ;
}

Client:: Client ( QString host ,
                  int     p    ,
                  QString uri  ,
                  int     T    )
       : Talk   (              )
{
  Information [ "Keep-Alive"  ] =     0        ;
  Information [ "Selecting"   ] = 25000        ;
  Information [ "Exception"   ] =     0        ;
  Information [ "Exceptions"  ] =    10        ;
  Information [ "Timeout"     ] = 60000        ;
  Information [ "AcceptFault" ] = false        ;
  Information [ "URI"         ] = "/RPC2"      ;
  state                         = NoConnection ;
  executing                     = false        ;
  port                          = p            ;
  ExternalData                  = NULL         ;
  ExternalIO                    = NULL         ;
  if ( uri  . length ( ) > 0 )                 {
    Information [ "URI"      ]  = uri          ;
  }                                            ;
  if ( host . length ( ) > 0 )                 {
    Information [ "Hostname" ]  = host         ;
  }                                            ;
  setCommunication ( T )                       ;
}

Client::~Client (void)
{
  Close      ( ) ;
  RemoveComm ( ) ;
}

Header & Client::HttpHeader(void)
{
  return header ;
}

int Client::setPort(int p)
{
  port = p ;
  return p ;
}

bool Client::setHostname(QString host)
{
  Information [ "Hostname" ]  = host ;
  return true                        ;
}

bool Client::setURI(QString uri)
{
  Information [ "URI" ]  = uri ;
  return true                  ;
}

void Client::destroyer(void)
{
  delete this ;
}

void Client::TalkError(int level,int code)
{
  ClientError ( level , code ) ;
}

void Client::ClientError(int level,int code)
{
}

bool Client::EnsureConnected(void)
{
  if (   isConnected ( ) )                                               {
    state = Connected                                                    ;
    return true                                                          ;
  }                                                                      ;
  state = NoConnection                                                   ;
  if ( ! Lookup      ( ) ) return false                                  ;
  state = Connecting                                                     ;
  if (   ConnectTo   ( ) )                                               {
    int rbs                   = Information [ "ReadBuffer" ] . toInt ( ) ;
    state                     = Connected                                ;
    header                    . clear ( )                                ;
    pending                   . clear ( )                                ;
    http                      = ""                                       ;
    request                   = ""                                       ;
    response                  = ""                                       ;
    Information [ "Lastest" ] = QDateTime::currentDateTime ( )           ;
    ReadBuffer                . resize        ( rbs + 1       )          ;
    SendBuffer                . clear         (               )          ;
    Information               . remove ( "ErrorCode"   )                 ;
    Information               . remove ( "ErrorString" )                 ;
    ReturnBool                = false                                    ;
    return true                                                          ;
  } else                                                                 {
    state = NoConnection                                                 ;
  }                                                                      ;
  return false                                                           ;
}

bool Client::isRunning(void)
{
  return executing ;
}

bool Client::TryClose(void)
{
  if ( ! isConnected ( ) )                             {
    Close      ( )                                     ;
    RemoveComm ( )                                     ;
    state = NoConnection                               ;
    return true                                        ;
  }                                                    ;
  if ( Information [ "Keep-Alive" ] . toInt ( ) <= 0 ) {
    Close      ( )                                     ;
    RemoveComm ( )                                     ;
    state = NoConnection                               ;
    return true                                        ;
  }                                                    ;
  return false                                         ;
}

bool Client::HeaderCompleted(void)
{
  return header . parse ( pending ) ;
}

bool Client::ContentCompleted(void)
{
  if ( http . length ( ) <= 0 ) return false ;
  qint64 length = header . Length ( )        ;
  if ( length < 0             ) return false ;
  return ( pending . size ( ) >= length )    ;
}

bool Client::execute(void)
{
  switch ( ExecuteID )              {
    case 0                          :
    return executeMethod        ( ) ;
    case 1                          :
    return executeInstance      ( ) ;
    case 2                          :
    return transmitMethodData   ( ) ;
    case 3                          :
    return transmitInstanceData ( ) ;
    case 4                          :
    return transmitMethodIO     ( ) ;
    case 5                          :
    return transmitInstanceIO   ( ) ;
    case 6                          :
    return receiveMethodData    ( ) ;
    case 7                          :
    return receiveInstanceData  ( ) ;
    case 8                          :
    return receiveMethodIO      ( ) ;
    case 9                          :
    return receiveInstanceIO    ( ) ;
  }                                 ;
  return false                      ;
}

bool Client::executeInstance(void)
{
  ReturnBool = execute ( UseInstance , Parameters , Results ) ;
  return ReturnBool                                           ;
}

bool Client::execute (qint64 i,Value & p,Value & r)
{
  if ( executing )                          {
    ClientError ( 45 , 40001 )              ;
    return false                            ;
  }                                         ;
  PreventReEntry PRE ( executing )          ;
  executing = true                          ;
  ///////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false ;
  ///////////////////////////////////////////
  QString c                                 ;
  c          = CreateRequest ( i , p )      ;
  SendBuffer = c . toUtf8    (       )      ;
  ///////////////////////////////////////////
  return execute ( r )                      ;
}

bool Client::executeMethod(void)
{
  ReturnBool = execute ( UseMethod , Parameters , Results ) ;
  return ReturnBool                                         ;
}

bool Client::execute (QString m,Value & p,Value & r)
{
  if ( executing )                          {
    ClientError ( 45 , 40001 )              ;
    return false                            ;
  }                                         ;
  PreventReEntry PRE ( executing )          ;
  executing = true                          ;
  ///////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false ;
  ///////////////////////////////////////////
  QString c                                 ;
  c          = CreateRequest ( m , p )      ;
  SendBuffer = c . toUtf8    (       )      ;
  ///////////////////////////////////////////
  return execute ( r )                      ;
}

bool Client::execute (const char * m,Value & p,Value & r)
{
  if ( NULL == m           ) return false ;
  QString M = QString::fromUtf8 ( m )     ;
  if ( M . length ( ) <= 0 ) return false ;
  return execute ( M , p , r )            ;
}

bool Client::transmitInstanceData (void)
{
  ReturnBool = false                                               ;
  if ( NULL == ExternalData ) return false                         ;
  ReturnBool = transmitting ( UseInstance , Http , *ExternalData ) ;
  return ReturnBool                                                ;
}

bool Client::transmitting (qint64 i,Header & p,QByteArray & d)
{
  if ( executing )                                {
    ClientError ( 45 , 40001 )                    ;
    return false                                  ;
  }                                               ;
  PreventReEntry PRE ( executing )                ;
  executing = true                                ;
  /////////////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false       ;
  /////////////////////////////////////////////////
  p . addItem ( "Instance" , QString::number(i) ) ;
  p . addItem ( "Invoke"   , "Instance"         ) ;
  /////////////////////////////////////////////////
  return transmitting ( p , d )                   ;
}

bool Client::transmitMethodData (void)
{
  ReturnBool = false                                             ;
  if ( NULL == ExternalData ) return false                       ;
  ReturnBool = transmitting ( UseMethod , Http , *ExternalData ) ;
  return ReturnBool                                              ;
}

bool Client::transmitting (QString m,Header & p,QByteArray & d)
{
  if ( executing )                          {
    ClientError ( 45 , 40001 )              ;
    return false                            ;
  }                                         ;
  PreventReEntry PRE ( executing )          ;
  executing = true                          ;
  ///////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false ;
  ///////////////////////////////////////////
  p . addItem ( "Method" , m        )       ;
  p . addItem ( "Invoke" , "Method" )       ;
  ///////////////////////////////////////////
  return transmitting ( p , d )             ;
}

bool Client::transmitting (const char * m,Header & p,QByteArray & d)
{
  if ( NULL == m           ) return false ;
  QString M = QString::fromUtf8 ( m )     ;
  if ( M . length ( ) <= 0 ) return false ;
  return transmitting ( M , p , d )       ;
}

bool Client::transmitInstanceIO(void)
{
  ReturnBool = false                                             ;
  if ( NULL == ExternalIO ) return false                         ;
  ReturnBool = transmitting ( UseInstance , Http , *ExternalIO ) ;
  return ReturnBool                                              ;
}

bool Client::transmitting (qint64 i,Header & p,QIODevice & io)
{
  if ( executing )                                {
    ClientError ( 45 , 40001 )                    ;
    return false                                  ;
  }                                               ;
  PreventReEntry PRE ( executing )                ;
  executing = true                                ;
  /////////////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false       ;
  /////////////////////////////////////////////////
  p . addItem ( "Instance" , QString::number(i) ) ;
  p . addItem ( "Invoke"   , "Instance"         ) ;
  /////////////////////////////////////////////////
  return transmitting ( p , io )                  ;
}

bool Client::transmitMethodIO(void)
{
  ReturnBool = false                                           ;
  if ( NULL == ExternalIO ) return false                       ;
  ReturnBool = transmitting ( UseMethod , Http , *ExternalIO ) ;
  return ReturnBool                                            ;
}

bool Client::transmitting (QString m,Header & p,QIODevice & io)
{
  if ( executing )                          {
    ClientError ( 45 , 40001 )              ;
    return false                            ;
  }                                         ;
  PreventReEntry PRE ( executing )          ;
  executing = true                          ;
  ///////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false ;
  ///////////////////////////////////////////
  p . addItem ( "Method" , m        )       ;
  p . addItem ( "Invoke" , "Method" )       ;
  ///////////////////////////////////////////
  return transmitting ( p , io )            ;
}

bool Client::transmitting (const char * m,Header & p,QIODevice & io)
{
  if ( NULL == m           ) return false ;
  QString M = QString::fromUtf8 ( m )     ;
  if ( M . length ( ) <= 0 ) return false ;
  return transmitting ( M , p , io )      ;
}

bool Client::receiveInstanceData(void)
{
  ReturnBool = false                                            ;
  if ( NULL == ExternalData ) return false                      ;
  ReturnBool = receiving ( UseInstance , Http , *ExternalData ) ;
  return ReturnBool                                             ;
}

bool Client::receiving(qint64 i,Header & p,QByteArray & d)
{
  if ( executing )                                {
    ClientError ( 45 , 40001 )                    ;
    return false                                  ;
  }                                               ;
  PreventReEntry PRE ( executing )                ;
  executing = true                                ;
  /////////////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false       ;
  /////////////////////////////////////////////////
  p . addItem ( "Instance" , QString::number(i) ) ;
  p . addItem ( "Invoke"   , "Instance"         ) ;
  /////////////////////////////////////////////////
  return receiving ( p , d )                      ;
}

bool Client::receiveMethodData(void)
{
  ReturnBool = false                                          ;
  if ( NULL == ExternalData ) return false                    ;
  ReturnBool = receiving ( UseMethod , Http , *ExternalData ) ;
  return ReturnBool                                           ;
}

bool Client::receiving(QString m,Header & p,QByteArray & d)
{
  if ( executing )                          {
    ClientError ( 45 , 40001 )              ;
    return false                            ;
  }                                         ;
  PreventReEntry PRE ( executing )          ;
  executing = true                          ;
  ///////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false ;
  ///////////////////////////////////////////
  p . addItem ( "Method" , m        )       ;
  p . addItem ( "Invoke" , "Method" )       ;
  ///////////////////////////////////////////
  return receiving ( p , d )                ;
}

bool Client::receiving(const char * m,Header & p,QByteArray & d)
{
  if ( NULL == m           ) return false ;
  QString M = QString::fromUtf8 ( m )     ;
  if ( M . length ( ) <= 0 ) return false ;
  return receiving ( M , p , d )          ;
}

bool Client::receiveInstanceIO(void)
{
  ReturnBool = false                                          ;
  if ( NULL == ExternalIO ) return false                      ;
  ReturnBool = receiving ( UseInstance , Http , *ExternalIO ) ;
  return ReturnBool                                           ;
}

bool Client::receiving(qint64 i,Header & p,QIODevice & io)
{
  if ( executing )                                {
    ClientError ( 45 , 40001 )                    ;
    return false                                  ;
  }                                               ;
  PreventReEntry PRE ( executing )                ;
  executing = true                                ;
  /////////////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false       ;
  /////////////////////////////////////////////////
  p . addItem ( "Instance" , QString::number(i) ) ;
  p . addItem ( "Invoke"   , "Instance"         ) ;
  /////////////////////////////////////////////////
  return receiving ( p , io )                     ;
}

bool Client::receiveMethodIO(void)
{
  ReturnBool = false                                        ;
  if ( NULL == ExternalIO ) return false                    ;
  ReturnBool = receiving ( UseMethod , Http , *ExternalIO ) ;
  return ReturnBool                                         ;
}

bool Client::receiving(QString m,Header & p,QIODevice & io)
{
  if ( executing )                          {
    ClientError ( 45 , 40001 )              ;
    return false                            ;
  }                                         ;
  PreventReEntry PRE ( executing )          ;
  executing = true                          ;
  ///////////////////////////////////////////
  if ( ! EnsureConnected ( ) ) return false ;
  ///////////////////////////////////////////
  p . addItem ( "Method" , m        )       ;
  p . addItem ( "Invoke" , "Method" )       ;
  ///////////////////////////////////////////
  return receiving ( p , io )               ;
}

bool Client::receiving(const char * m,Header & p,QIODevice & io)
{
  if ( NULL == m           ) return false ;
  QString M = QString::fromUtf8 ( m )     ;
  if ( M . length ( ) <= 0 ) return false ;
  return receiving ( M , p , io )         ;
}

bool Client::ParseResponse(QString & r,Value & v)
{
  int offset = 0                                            ;
  if ( ! v . findTag ( METHODRESPONSE_TAG , r , &offset ) ) {
    return false                                            ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  bool correct = false                                      ;
  if   ( v . nextTagIs ( PARAMS_TAG , r , &offset ) )       {
    if ( v . nextTagIs ( PARAM_TAG  , r , &offset ) )       {
      correct = true                                        ;
    }                                                       ;
  } else                                                    {
    if  ( Information [ "AcceptFault" ] . toBool ( ) )      {
      if ( v . nextTagIs ( FAULT_TAG  , r , &offset ) )     {
        correct = true                                      ;
      }                                                     ;
    }                                                       ;
  }                                                         ;
  if ( ! correct ) return false                             ;
  ///////////////////////////////////////////////////////////
  if ( ! v . fromXml ( r , &offset ) ) return false         ;
  return v . valid ( )                                      ;
}

QString Client::CreateRequest(QString m,Value & p)
{
  Header     h = header                               ;
  QString    r = GenerateRequest ( m , p            ) ;
  QByteArray b = r . toUtf8      (                  ) ;
  QString    t = GenerateHeader  ( h , b . size ( ) ) ;
  return                         ( t + r            ) ;
}

QString Client::CreateRequest(qint64 i,Value & p)
{
  Header     h = header                                        ;
  QString    I = QString::number ( i                         ) ;
  //////////////////////////////////////////////////////////////
  h            . addItem         ( "Instance" , I            ) ;
  h            . addItem         ( "Invoke"   , "Instance"   ) ;
  //////////////////////////////////////////////////////////////
  QString    r = GenerateRequest ( "Instance" , p            ) ;
  QByteArray b = r . toUtf8      (                           ) ;
  QString    t = GenerateHeader  ( h          , b . size ( ) ) ;
  return                         ( t + r                     ) ;
}

QString Client::GenerateHeader(Header & p,qint64 s)
{
  QString uri  = Information [ "URI"        ] . toString ( )           ;
  QString host = Information [ "Hostname"   ] . toString ( )           ;
  int     ka   = Information [ "Keep-Alive" ] . toInt    ( )           ;
  QString h                                                            ;
  p . addLength        ( s                                           ) ;
  p . addMime          ( "text/xml"                                  ) ;
  p . addItem          ( "User-Agent" , QString::fromUtf8 ( XMLRPC ) ) ;
  p . addItem          ( "Host"       , host                         ) ;
  p . addItem          ( "Port"       , QString::number ( port )     ) ;
  p . addItem          ( "Keep-Alive" , QString::number ( ka   )     ) ;
  if ( Information . contains ( "Alias" ) )                            {
    QString alias = Information [ "Alias" ] . toString ( )             ;
    p        . addItem          ( "Alias"       , alias              ) ;
  }                                                                    ;
  h  = QString         ( "POST %1 HTTP/1.1\r\n" ) . arg ( uri  )       ;
  h += p . httpHeader  (                                             ) ;
  return h                                                             ;
}

QString Client::GenerateRequest(QString methodName,Value & params)
{
  QString body                                          ;
  body   = QString::fromUtf8 ( REQUEST_BEGIN          ) ;
  body  += methodName                                   ;
  body  += QString::fromUtf8 ( REQUEST_END_METHODNAME ) ;
  ///////////////////////////////////////////////////////
  if ( params . valid ( ) )                             {
    body += QString::fromUtf8 ( PARAMS_TAG  )           ;
    body += QString::fromUtf8 ( PARAM_TAG   )           ;
    body += params . toXml    (             )           ;
    body += QString::fromUtf8 ( PARAM_ETAG  )           ;
    body += QString::fromUtf8 ( PARAMS_ETAG )           ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  body   += QString::fromUtf8 ( REQUEST_END )           ;
  ///////////////////////////////////////////////////////
  return body                                           ;
}

bool Client::execute(Value & r)
{
  if ( SendBuffer . size ( ) <= 0 ) return false                             ;
  bool     correct = false                                                   ;
  int      lstate  = 901                                                     ;
  bool     go      = true                                                    ;
  int      rt                                                                ;
  state            = WriteRequest                                            ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && isContinue ( ) )                                             {
    switch ( state )                                                         {
      case NoConnection                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case Connecting                                                        :
      break                                                                  ;
      case Connected                                                         :
      break                                                                  ;
      case Idle                                                              :
      break                                                                  ;
      case WriteRequest                                                      :
        if ( SendBuffer . size ( ) <= 0 )                                    {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
        } else                                                               {
          lstate = state                                                     ;
          state  = 902                                                       ;
          TeaTime ( 1 , state )                                              ;
        }                                                                    ;
      break                                                                  ;
      case ReadHeader                                                        :
        if ( HeaderCompleted ( ) )                                           {
          QByteArray HTTP                                                    ;
          rt   = header . splitter ( pending )                               ; /*! Take HTTP header */
          http = ""                                                          ;
          if ( rt > 0 )                                                      {
            HTTP = pending . mid     ( 0 , rt )                              ;
            http = QString::fromUtf8 ( HTTP   )                              ;
          }                                                                  ;
          rt = header . content  ( pending )                                 ; /*! Remove HTTP header */
          if ( rt > 0 ) pending . remove ( 0 , rt )                          ;
          state = ReadResponse                                               ;
          TeaTime ( 1 , state )                                              ;
        } else                                                               {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
          TeaTime ( 1 , lstate )                                             ;
        }                                                                    ;
      break                                                                  ;
      case ReceiveData                                                       :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case TransmitData                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case ReadResponse                                                      :
        if ( ContentCompleted ( ) )                                          {
          QByteArray R = pending . mid ( 0 , header . Length ( ) )           ;
          request      = QString::fromUtf8 ( R )                             ;
          pending      . remove        ( 0 , header . Length ( ) )           ;
          r            . clear         (                         )           ;
          correct      = ParseResponse ( request , r             )           ;
          go           = false                                               ;
          state        = Connected                                           ;
        } else                                                               {
          state  = 901                                                       ;
          lstate = ReadResponse                                              ;
          TeaTime ( 1 , state )                                              ;
        }                                                                    ;
      break                                                                  ;
      case 901 /*! Receive data */                                           :
        if ( ! PourIn ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 1 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      case 902 /*! Actually write to peer side */                            :
        if ( ! ToPeer ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 1 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      default                                                                :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  TryClose ( )                                                               ;
  return correct                                                             ;
}

bool Client::transmitting(Header & p,QByteArray & d)
{
  QString  uri     = Information [ "URI"        ] . toString ( )             ;
  QString  host    = Information [ "Hostname"   ] . toString ( )             ;
  int      ka      = Information [ "Keep-Alive" ] . toInt    ( )             ;
  bool     correct = false                                                   ;
  int      lstate  = 901                                                     ;
  bool     go      = true                                                    ;
  QString  h                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  p          . addItem          ( "Device" , "Memory"                      ) ;
  p          . addLength        ( d . size ( )                             ) ;
  p          . addItem          ( "User-Agent" , QString::fromUtf8(XMLRPC) ) ;
  p          . addItem          ( "Host"       , host                      ) ;
  p          . addItem          ( "Port"       , QString::number ( port )  ) ;
  p          . addItem          ( "Keep-Alive" , QString::number ( ka   )  ) ;
  if ( Information . contains ( "Alias" ) )                                  {
    QString alias = Information [ "Alias" ] . toString ( )                   ;
    p        . addItem          ( "Alias"       , alias                    ) ;
  }                                                                          ;
  h          = QString          ( "PUT %1 HTTP/1.1\r\n" ) . arg ( uri  )     ;
  h         += p . httpHeader   (                                          ) ;
  SendBuffer = h . toUtf8       (                                          ) ;
  SendBuffer . append           ( d                                        ) ;
  state      = TransmitData                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && isContinue ( ) )                                             {
    switch ( state )                                                         {
      case NoConnection                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case Connecting                                                        :
      break                                                                  ;
      case Connected                                                         :
      break                                                                  ;
      case Idle                                                              :
      break                                                                  ;
      case WriteRequest                                                      :
      break                                                                  ;
      case ReadHeader                                                        :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case ReceiveData                                                       :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case TransmitData                                                      :
        if ( SendBuffer . size ( ) <= 0 )                                    {
          go      = false                                                    ;
          correct = true                                                     ;
        } else                                                               {
          lstate = state                                                     ;
          state  = 902                                                       ;
        }                                                                    ;
      break                                                                  ;
      case ReadResponse                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case 901 /*! Receive data */                                           :
        if ( ! PourIn ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 2 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      case 902 /*! Actually write to peer side */                            :
        if ( ! ToPeer ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 2 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      default                                                                :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  TryClose ( )                                                               ;
  return correct                                                             ;
}

bool Client::transmitting(Header & p,QIODevice & io)
{
  QString  uri     = Information [ "URI"        ] . toString ( )             ;
  QString  host    = Information [ "Hostname"   ] . toString ( )             ;
  int      ka      = Information [ "Keep-Alive" ] . toInt    ( )             ;
  bool     correct = false                                                   ;
  int      lstate  = 901                                                     ;
  bool     go      = true                                                    ;
  qint64   total   = 0                                                       ;
  qint64   sent    = 0                                                       ;
  qint64   fetch   = 0                                                       ;
  QString  h                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if                            ( p . Tags . contains ( "Content-Length" ) ) {
    total = p  . Length         (                                          ) ;
  } else                                                                     {
    total = io . size           (                                          ) ;
  }                                                                          ;
  p          . addLength        ( total                                    ) ;
  p          . addItem          ( "Device" , "IO"                          ) ;
  p          . addItem          ( "User-Agent" , QString::fromUtf8(XMLRPC) ) ;
  p          . addItem          ( "Host"       , host                      ) ;
  p          . addItem          ( "Port"       , QString::number ( port )  ) ;
  p          . addItem          ( "Keep-Alive" , QString::number ( ka   )  ) ;
  if ( Information . contains ( "Alias" ) )                                  {
    QString alias = Information [ "Alias" ] . toString ( )                   ;
    p        . addItem          ( "Alias"       , alias                    ) ;
  }                                                                          ;
  h          = QString          ( "PUT %1 HTTP/1.1\r\n" ) . arg ( uri  )     ;
  h         += p . httpHeader   (                                          ) ;
  SendBuffer = h . toUtf8       (                                          ) ;
  state      = TransmitData                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && isContinue ( ) )                                             {
    switch ( state )                                                         {
      case NoConnection                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case Connecting                                                        :
      break                                                                  ;
      case Connected                                                         :
      break                                                                  ;
      case Idle                                                              :
      break                                                                  ;
      case WriteRequest                                                      :
      break                                                                  ;
      case ReadHeader                                                        :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case ReceiveData                                                       :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case TransmitData                                                      :
        if ( SendBuffer . size ( ) >= Information["MaxWrite"].toInt ( ) )    {
          lstate = state                                                     ;
          state  = 902                                                       ;
        } else                                                               {
          if ( total > sent )                                                {
            QByteArray B                                                     ;
            fetch = total - sent                                             ;
            if ( fetch > Information [ "MaxWrite" ] . toInt ( ) )            {
              fetch = Information [ "MaxWrite" ] . toInt ( )                 ;
            }                                                                ;
            B     = io . read ( fetch )                                      ;
            fetch = B  . size (       )                                      ;
            if ( fetch > 0 )                                                 {
              SendBuffer . append ( B )                                      ;
              sent += fetch                                                  ;
            }                                                                ;
          }                                                                  ;
          if ( SendBuffer . size ( ) > 0 )                                   {
            lstate = state                                                   ;
            state  = 902                                                     ;
          } else                                                             {
            if ( sent >= total )                                             {
              go      = false                                                ;
              correct = true                                                 ;
            }                                                                ;
          }                                                                  ;
        }                                                                    ;
      break                                                                  ;
      case ReadResponse                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case 901 /*! Receive data */                                           :
        if ( ! PourIn ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 3 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      case 902 /*! Actually write to peer side */                            :
        if ( ! ToPeer ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 3 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      default                                                                :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  TryClose ( )                                                               ;
  return correct                                                             ;
}

bool Client::receiving(Header & p,QByteArray & d)
{
  QString  uri      = Information [ "URI"        ] . toString ( )            ;
  QString  host     = Information [ "Hostname"   ] . toString ( )            ;
  int      ka       = Information [ "Keep-Alive" ] . toInt    ( )            ;
  bool     correct  = false                                                  ;
  int      lstate   = 901                                                    ;
  bool     go       = true                                                   ;
  qint64   total    = 0                                                      ;
  qint64   accepted = 0                                                      ;
  int      rt                                                                ;
  QString  h                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  p          . addItem          ( "Device"     , "Memory"                  ) ;
  p          . addItem          ( "User-Agent" , QString::fromUtf8(XMLRPC) ) ;
  p          . addItem          ( "Host"       , host                      ) ;
  p          . addItem          ( "Port"       , QString::number ( port )  ) ;
  p          . addItem          ( "Keep-Alive" , QString::number ( ka   )  ) ;
  if ( Information . contains ( "Alias" ) )                                  {
    QString alias = Information [ "Alias" ] . toString ( )                   ;
    p        . addItem          ( "Alias"       , alias                    ) ;
  }                                                                          ;
  h          = QString          ( "GET %1 HTTP/1.1\r\n" ) . arg ( uri  )     ;
  h         += p . httpHeader   (                                          ) ;
  SendBuffer = h . toUtf8       (                                          ) ;
  state      = WriteRequest                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && isContinue ( ) )                                             {
    switch ( state )                                                         {
      case NoConnection                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case Connecting                                                        :
      break                                                                  ;
      case Connected                                                         :
      break                                                                  ;
      case Idle                                                              :
      break                                                                  ;
      case WriteRequest                                                      :
        if ( SendBuffer . size ( ) <= 0 )                                    {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
        } else                                                               {
          lstate = state                                                     ;
          state  = 902                                                       ;
        }                                                                    ;
      break                                                                  ;
      case ReadHeader                                                        :
        if ( HeaderCompleted ( ) )                                           {
          QByteArray HTTP                                                    ;
          /*! Take HTTP header */
          rt   = header . splitter ( pending )                               ;
          http = ""                                                          ;
          if ( rt > 0 )                                                      {
            HTTP = pending . mid     ( 0 , rt )                              ;
            http = QString::fromUtf8 ( HTTP   )                              ;
          }                                                                  ;
          /*! Remove HTTP header */
          rt = header . content  ( pending )                                 ;
          if ( rt > 0 ) pending . remove ( 0 , rt )                          ;
          total = header . Length ( )                                        ;
          state = ReceiveData                                                ;
        } else                                                               {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
        }                                                                    ;
      break                                                                  ;
      case ReceiveData                                                       :
        if   ( accepted >= total )                                           {
          correct = true                                                     ;
          go      = false                                                    ;
        } else                                                               {
          qint64 rs = pending . size ( )                                     ;
          if ( rs > 0 )                                                      {
            if ( total >= ( rs + accepted ) )                                {
              d         . append ( pending )                                 ;
              pending   . clear  (         )                                 ;
              accepted += rs                                                 ;
            } else                                                           {
              QByteArray B                                                   ;
              if ( rs > ( total - accepted ) ) rs = total - accepted         ;
              B = pending . mid    ( 0 , rs )                                ;
              pending     . remove ( 0 , rs )                                ;
              d           . append ( B      )                                ;
              accepted += rs                                                 ;
            }                                                                ;
          }                                                                  ;
          if ( accepted <  total )                                           {
            state   = 901                                                    ;
            lstate  = ReceiveData                                            ;
          }                                                                  ;
        }                                                                    ;
      break                                                                  ;
      case TransmitData                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case ReadResponse                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case 901 /*! Receive data */                                           :
        if ( ! PourIn ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 4 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      case 902 /*! Actually write to peer side */                            :
        if ( ! ToPeer ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 4 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      default                                                                :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  TryClose ( )                                                               ;
  return correct                                                             ;
}

bool Client::receiving(Header & p,QIODevice & io)
{
  QString  uri      = Information [ "URI"        ] . toString ( )            ;
  QString  host     = Information [ "Hostname"   ] . toString ( )            ;
  int      ka       = Information [ "Keep-Alive" ] . toInt    ( )            ;
  bool     correct  = false                                                  ;
  int      lstate   = 901                                                    ;
  bool     go       = true                                                   ;
  qint64   total    = 0                                                      ;
  qint64   accepted = 0                                                      ;
  int      rt                                                                ;
  QString  h                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  p          . addItem          ( "Device"     , "IO"                      ) ;
  p          . addItem          ( "User-Agent" , QString::fromUtf8(XMLRPC) ) ;
  p          . addItem          ( "Host"       , host                      ) ;
  p          . addItem          ( "Port"       , QString::number ( port )  ) ;
  p          . addItem          ( "Keep-Alive" , QString::number ( ka   )  ) ;
  if ( Information . contains ( "Alias" ) )                                  {
    QString alias = Information [ "Alias" ] . toString ( )                   ;
    p        . addItem          ( "Alias"       , alias                    ) ;
  }                                                                          ;
  h          = QString          ( "GET %1 HTTP/1.1\r\n" ) . arg ( uri  )     ;
  h         += p . httpHeader   (                                          ) ;
  SendBuffer = h . toUtf8       (                                          ) ;
  state      = WriteRequest                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && isContinue ( ) )                                             {
    switch ( state )                                                         {
      case NoConnection                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case Connecting                                                        :
      break                                                                  ;
      case Connected                                                         :
      break                                                                  ;
      case Idle                                                              :
      break                                                                  ;
      case WriteRequest                                                      :
        if ( SendBuffer . size ( ) <= 0 )                                    {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
        } else                                                               {
          lstate = state                                                     ;
          state  = 902                                                       ;
        }                                                                    ;
      break                                                                  ;
      case ReadHeader                                                        :
        if ( HeaderCompleted ( ) )                                           {
          QByteArray HTTP                                                    ;
          /*! Take HTTP header */
          rt   = header . splitter ( pending )                               ;
          http = ""                                                          ;
          if ( rt > 0 )                                                      {
            HTTP = pending . mid     ( 0 , rt )                              ;
            http = QString::fromUtf8 ( HTTP   )                              ;
          }                                                                  ;
          /*! Remove HTTP header */
          rt = header . content  ( pending )                                 ;
          if ( rt > 0 ) pending . remove ( 0 , rt )                          ;
          total = header . Length ( )                                        ;
          state = ReceiveData                                                ;
        } else                                                               {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
        }                                                                    ;
      break                                                                  ;
      case ReceiveData                                                       :
        if   ( accepted >= total )                                           {
          correct = true                                                     ;
          go      = false                                                    ;
        } else                                                               {
          qint64 rs = pending . size ( )                                     ;
          if ( rs > 0 )                                                      {
            if ( total >= ( rs + accepted ) )                                {
              rs        = io . write ( pending )                             ;
              pending   . remove     ( 0 , rs  )                             ;
              accepted += rs                                                 ;
            } else                                                           {
              QByteArray B                                                   ;
              if ( rs > ( total - accepted ) ) rs = total - accepted         ;
              B  = pending . mid    ( 0 , rs )                               ;
              rs = io      . write  ( B      )                               ;
              pending      . remove ( 0 , rs )                               ;
              accepted += rs                                                 ;
            }                                                                ;
          }                                                                  ;
          if ( accepted <  total )                                           {
            state   = 901                                                    ;
            lstate  = ReceiveData                                            ;
          }                                                                  ;
        }                                                                    ;
      break                                                                  ;
      case TransmitData                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case ReadResponse                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case 901 /*! Receive data */                                           :
        if ( ! PourIn ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 5 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      case 902 /*! Actually write to peer side */                            :
        if ( ! ToPeer ( ) )                                                  {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
        TeaTime ( 5 , lstate )                                               ;
        state = lstate                                                       ;
      break                                                                  ;
      default                                                                :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  TryClose ( )                                                               ;
  return correct                                                             ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
