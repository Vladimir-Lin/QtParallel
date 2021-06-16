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

#pragma pack(push,1)

typedef struct               {
  int                Type    ;
} AskListener                ;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////

char          METHODNAME_TAG  [ ] = "<methodName>"                           ;
char          METHODNAME_ETAG [ ] = "</methodName>"                          ;
extern char   PARAMS_TAG      [ ]                                            ;
extern char   PARAMS_ETAG     [ ]                                            ;
extern char   PARAM_TAG       [ ]                                            ;
extern char   PARAM_ETAG      [ ]                                            ;
extern char   XMLRPC          [ ]                                            ;
const QString METHODNAME          = QString ( "methodName"                 ) ;
const QString PARAMS              = QString ( "params"                     ) ;
const QString FAULTCODE           = QString ( "faultCode"                  ) ;
const QString FAULTSTRING         = QString ( "faultString"                ) ;
const QString FaultRESPONSE_1     = QString ( "<?xml version=\"1.0\"?>"
                                              "\r\n"
                                              "<methodResponse><fault>"
                                              "\r\n"                       ) ;
const QString FaultRESPONSE_2     = QString ( "\r\n"
                                              "</fault></methodResponse>"
                                              "\r\n"                       ) ;
const QString HeaderRESPONSE_1    = QString ( "<?xml version=\"1.0\"?>"
                                              "\r\n"
                                              "<methodResponse><params><param>"
                                              "\r\n"                       ) ;
const QString HeaderRESPONSE_2    = QString ( "\r\n"
                                              "</param></params></methodResponse>"
                                              "\r\n"                       ) ;

//////////////////////////////////////////////////////////////////////////////

Server:: Server  ( void )
       : Talk    (      )
       , methods ( NULL )
{
  Information [ "Keep-Alive" ] =     0        ;
  Information [ "Selecting"  ] = 25000        ;
  Information [ "Exception"  ] =     0        ;
  Information [ "Exceptions" ] =    10        ;
  Information [ "Timeout"    ] = 60000        ;
  state                        = NoConnection ;
}

Server:: Server  ( Methods * m , void * incoming )
       : Talk    (                               )
       , methods (           m                   )
{
  Information [ "Keep-Alive" ] =     0        ;
  Information [ "Selecting"  ] = 25000        ;
  Information [ "Exception"  ] =     0        ;
  Information [ "Exceptions" ] =    10        ;
  Information [ "Timeout"    ] = 60000        ;
  state                        = NoConnection ;
  Accept ( incoming )                         ;
}

Server::~Server (void)
{
  methods = NULL ;
  Close      ( ) ;
  RemoveComm ( ) ;
}

void Server::destroyer(void)
{
  delete this ;
}

void Server::TalkError(int level,int code)
{
  ServerError ( level , code ) ;
}

void Server::ServerError(int level,int code)
{
  if ( NULL == methods ) return           ;
  methods -> MethodError ( level , code ) ;
}

void Server::setMethods(Methods * m)
{
  methods = m ;
}

bool Server::Accept(void * incoming)
{
  if ( NULL == incoming ) return false        ;
  AskListener * al = (AskListener *) incoming ;
  switch ( al -> Type )                       {
    case CommNothing                          :
      state = NoConnection                    ;
    return false                              ;
    case CommIPv4                             :
      state = Idle                            ;
    break                                     ;
    case CommIPv6                             :
      state = NoConnection                    ;
    return false                              ;
    case CommZMQ                              :
      state = NoConnection                    ;
    return false                              ;
  }                                           ;
  return Accepted ( incoming )                ;
}

bool Server::Prepare(void)
{
  if ( NULL == comm ) return false        ;
  AskListener * al = (AskListener *) comm ;
  switch ( al -> Type )                   {
    case CommNothing                      :
    return false                          ;
    case CommIPv4                         :
    return PrepareTcp ( )                 ;
    case CommIPv6                         :
    return false                          ;
    case CommZMQ                          :
    return false                          ;
  }                                       ;
  return true                             ;
}

bool Server::Watching(void)
{
  if ( NULL == comm ) return false        ;
  AskListener * al = (AskListener *) comm ;
  switch ( al -> Type )                   {
    case CommNothing                      :
    return false                          ;
    case CommIPv4                         :
    return WatchTcp ( )                   ;
    case CommIPv6                         :
    return false                          ;
    case CommZMQ                          :
    return false                          ;
  }                                       ;
  return true                             ;
}

bool Server::NormalWatch(void)
{
  Method    * m       = NULL                                                 ;
  int         lstate  = 901                                                  ;
  bool        go      = true                                                 ;
  bool        correct = true                                                 ;
  bool        rr                                                             ;
  int         rt                                                             ;
  qint64      plen                                                           ;
  QString     methodName                                                     ;
  Value       params                                                         ;
  Value       results                                                        ;
  Header      h                                                              ;
  QStringList TAGs                                                           ;
  ////////////////////////////////////////////////////////////////////////////
  TAGs << "Keep-Alive"                                                       ;
  TAGs << "Cleanup"                                                          ;
  TAGs << "Timeout"                                                          ;
  TAGs << "ReadBuffer"                                                       ;
  TAGs << "MaxWrite"                                                         ;
  TAGs << "SendingBuffer"                                                    ;
  TAGs << "ReceiveBuffer"                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && isContinue ( ) )                                             {
    switch ( state )                                                         {
      case NoConnection                                                      :
        go      = false                                                      ;
        correct = false                                                      ;
      break                                                                  ;
      case Idle                                                              :
        rt = Receive ( pending , Information [ "Selecting" ] . toInt ( ) )   ;
        if ( rt >  0 )                                                       {
          Information [ "Exception" ] = 0                                    ;
          state                       = ReadHeader                           ;
          TeaTime ( 1 , lstate )                                             ;
        } else if ( rt == 0 )                                                { /*! unlimit waiting */
          TeaTime ( 1 , Idle )                                               ;
        } else                                                               {
          Information["Exception"] = Information["Exception"].toInt() + 1    ;
          if ( Information [ "Exception"  ] . toInt ( )                      >
               Information [ "Exceptions" ] . toInt ( )                    ) {
            go      = false                                                  ;
            correct = false                                                  ;
          } else                                                             {
            TeaTime ( 1 , Idle )                                             ;
          }                                                                  ;
        }                                                                    ;
      break                                                                  ;
      case ReadHeader                                                        :
        if ( HeaderCompleted ( ) )                                           {
          QByteArray HTTP                                                    ;
          QString    tgs                                                     ;
          rt   = header . splitter ( pending )                               ; /*! Take HTTP header */
          http = ""                                                          ;
          if ( rt > 0 )                                                      {
            HTTP = pending . mid     ( 0 , rt )                              ;
            http = QString::fromUtf8 ( HTTP   )                              ;
          }                                                                  ;
          rt = header . content  ( pending )                                 ; /*! Remove HTTP header */
          if ( rt > 0 ) pending . remove ( 0 , rt )                          ;
          foreach ( tgs , TAGs )                                             { /*! Copy tags */
            if ( header . Tags . contains ( tgs ) )                          {
              Information [ tgs ] = header . Tags [ tgs ]                    ;
            }                                                                ;
          }                                                                  ;
          if ( header . Tags . contains ( "GET"  ) )                         { /*! Decide which way to go */
            m = FindMethod ( header )                                        ;
            if ( ( NULL != m ) && m -> capable ( Method::Transmitter ) )     { /*! Retrieve data from Server */
              state      = TransmitData                                      ;
              SendBuffer . clear ( )                                         ;
              h          . clear ( )                                         ;
              h          . Tags                   = header . Tags            ;
              h          . Tags [ "HeaderSent"  ] = false                    ;
              h          . Tags [ "HeaderReady" ] = false                    ;
            } else                                                           {
              go      = false                                                ;
              correct = false                                                ;
            }                                                                ;
          }                                                                  ;
          if ( header . Tags . contains ( "PUT"  ) )                         {
            m = FindMethod ( header )                                        ;
            if ( ( NULL != m ) && m -> capable ( Method::Receiver ) )        { /*! Transmit data to Server */
              state                         = ReceiveData                    ;
              header . Tags [ "Processed" ] = (qint64) 0                     ;
            } else                                                           {
              go      = false                                                ;
              correct = false                                                ;
            }                                                                ;
          }                                                                  ;
          if ( header . Tags . contains ( "POST" ) )                         {
            state = ReadRequest                                              ;
          }                                                                  ;
          TeaTime ( 1 , state )                                              ;
        } else                                                               {
          state  = 901                                                       ;
          lstate = ReadHeader                                                ;
          TeaTime ( 1 , lstate )                                             ;
        }                                                                    ;
      break                                                                  ;
      case ReadRequest                                                       :
        if ( ContentCompleted ( ) )                                          {
          QByteArray R = pending . mid ( 0 , header . Length ( ) )           ;
          request      = QString::fromUtf8 ( R )                             ;
          pending      . remove ( 0 , header . Length ( ) )                  ;
          params       . clear  (                         )                  ;
          methodName   = Parse  ( request , params        )                  ;
          state        = ExecuteRequest                                      ;
        } else                                                               {
          state  = 901                                                       ;
          lstate = ReadRequest                                               ;
          TeaTime ( 1 , state )                                              ;
        }                                                                    ;
      break                                                                  ;
      case ReceiveData                                                       :
        if ( NULL != m )                                                     {
          plen = header . Tags [ "Processed" ] . toLongLong ( )              ;
          if ( plen >= header . Length ( ) )                                 { /*! Data receiving completed */
            state  = 900                                                     ;
            lstate = ReadHeader                                              ;
          } else                                                             {
            qint64 ps = pending . size ( )                                   ;
            qint64 rs                                                        ;
            if ( m -> receiving ( header , pending ) )                       {
              rs = pending . size ( )                                        ;
              if ( ps > rs )                                                 {
                ps                           -= rs                           ;
                ps                           += plen                         ;
                header . Tags [ "Processed" ] = ps                           ;
              }                                                              ;
            } else                                                           {
              go      = false                                                ;
              correct = false                                                ;
            }                                                                ;
            state  = 901                                                     ;
            lstate = ReceiveData                                             ;
          }                                                                  ;
        } else                                                               {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
      break                                                                  ;
      case TransmitData                                                      :
        if ( NULL != m )                                                     {
          if   ( ! h . Tags [ "HeaderSent"  ] . toBool ( ) )                 {
            if ( ! h . Tags [ "HeaderReady" ] . toBool ( ) )                 {
              go = m -> transmitting ( h , SendBuffer )                      ;
            } else                                                           {
              QString    hh = QString ( "HTTP/1.1 200 OK\r\n" )              ;
              QByteArray BB                                                  ;
              hh += h  . httpHeader ( )                                      ;
              BB  = hh . toUtf8     ( )                                      ;
              h . Tags [ "HeaderSent" ] = true                               ;
              h . Tags [ "DataSent"   ] = false                              ;
              h . Tags [ "DataSize"   ] = SendBuffer . size ( )              ;
              SendBuffer . prepend ( BB )                                    ;
              state  = 902                                                   ;
              lstate = TransmitData                                          ;
              TeaTime ( 1 , lstate )                                         ;
            }                                                                ;
          } else                                                             {
            int mw = Information [ "MaxWrite" ] . toInt ( )                  ;
            if ( mw > SendBuffer . size ( ) )                                {
              if ( ! h . Tags [ "DataSent" ] . toBool ( ) )                  {
                go = m -> transmitting ( h , SendBuffer )                    ;
              }                                                              ;
            }                                                                ;
            if ( h . Tags [ "DataSent" ] . toBool ( ) )                      {
              if ( SendBuffer . size ( ) > 0 )                               {
                state  = 902                                                 ;
                lstate = TransmitData                                        ;
              } else                                                         {
                state  = 900                                                 ;
                lstate = ReadHeader                                          ;
              }                                                              ;
            } else                                                           {
              if ( SendBuffer . size ( ) > 0 )                               {
                state  = 902                                                 ;
                lstate = TransmitData                                        ;
              }                                                              ;
            }                                                                ;
          }                                                                  ;
        } else                                                               {
          go      = false                                                    ;
          correct = false                                                    ;
        }                                                                    ;
      break                                                                  ;
      case ExecuteRequest                                                    :
        //////////////////////////////////////////////////////////////////////
        response = ""                                                        ;
        results  . clear (   )                                               ;
        h        . clear (   )                                               ;
        BeforeResponse   ( h )                                               ;
        //////////////////////////////////////////////////////////////////////
        if ( methodName . length ( ) <= 0 )                                  {
          Information [ "ErrorCode"   ] = 30003                              ;
          Information [ "ErrorString" ] = QString ( "No method found" )      ;
          results = false                                                    ;
          rr      = false                                                    ;
        } else                                                               {
          if ( header . isInvoke ( "Instance" ) )                            {
            qint64 inst = header . Tags [ "Instance" ] . toLongLong ( )      ;
            rr = execute ( inst                , params , results )          ;
          } else                                                             {
            rr = execute ( methodName , header , params , results )          ;
          }                                                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( rr )                                                            {
          h . addItem ( "Answer" , "Correct" )                               ;
          response = GenerateResponse ( h , results . toXml ( ) )            ;
        } else                                                               {
          h . addItem ( "Answer" , "Fault"   )                               ;
          response = FaultResponse                                           (
                       h                                                     ,
                       Information [ "ErrorString" ] . toString ( )          ,
                       Information [ "ErrorCode"   ] . toInt    ( )        ) ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( response . length ( ) > 0 )                                     {
          SendBuffer = response . toUtf8 ( )                                 ;
          state      = WriteResponse                                         ;
        } else                                                               {
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        TeaTime ( 1 , state )                                                ;
      break                                                                  ;
      case WriteResponse                                                     :
        if ( SendBuffer . size ( ) <= 0 )                                    {
          state  = 900                                                       ;
          lstate = ReadHeader                                                ;
          TeaTime ( 1 , lstate )                                             ;
        } else                                                               {
          lstate = state                                                     ;
          state  = 902                                                       ;
        }                                                                    ;
      break                                                                  ;
      case 900 /*! Prepare for ReadHeader */                                 :
        if ( Information [ "Keep-Alive" ] . toInt ( ) > 0 )                  { /*! Response XML written, plan to the next step */
          state = lstate                                                     ;
          if ( header . Tags . contains ( "Loyality" ) )                     {
            if ( header . Tags [ "Loyality" ] . toInt ( ) > 0 )              { /*! If assign loyality state, go to unlimited waiting */
              state = Idle                                                   ;
            }                                                                ;
          }                                                                  ;
          /*! clear variables about to be reused                            */
          if ( Information [ "Cleanup" ] . toBool ( ) ) pending . clear ( )  ;
          http       . clear ( )                                             ;
          request    . clear ( )                                             ;
          response   . clear ( )                                             ;
          header     . clear ( )                                             ;
          SendBuffer . clear ( )                                             ;
          m          = NULL                                                  ;
        } else                                                               { /*! If not Keep-Alive, try to close this section */
          TeaTime ( 1 , 0    )                                               ;
          go = false                                                         ;
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
  Close ( )                                                                  ;
  return correct                                                             ;
}

bool Server::execute(QString n,Header & h,Value & p,Value & r)
{
  Method * m = NULL                                                  ;
  if ( NULL != methods ) m = methods -> find ( n , h )               ;
  if ( NULL == m )                                                   {
    Information [ "ErrorCode"   ] = 30003                            ;
    Information [ "ErrorString" ] = QString ( "No method found" )    ;
    r = (bool) false                                                 ;
    return false                                                     ;
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  bool e = m -> execute ( p , r )                                    ;
  if ( ! e )                                                         {
    Information [ "ErrorCode"   ] = 30001                            ;
    Information [ "ErrorString" ] = QString ( "Execution failure"  ) ;
    r = false                                                        ;
  } else if ( ! r . valid ( ) )                                      {
    Information [ "ErrorCode"   ] = 30002                            ;
    Information [ "ErrorString" ] = QString ( "Invalid Parameters" ) ;
    r = false                                                        ;
    e = false                                                        ;
  }                                                                  ;
  return e                                                           ;
}

bool Server::execute(qint64 n,Value & p,Value & r)
{
  Method * m = NULL                                                  ;
  if ( NULL != methods ) m = methods -> find ( n )                   ;
  if ( NULL == m )                                                   {
    Information [ "ErrorCode"   ] = 30004                            ;
    Information [ "ErrorString" ] = QString ( "No instance found" )  ;
    r = (bool) false                                                 ;
    return false                                                     ;
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  bool e = m -> execute ( p , r )                                    ;
  if ( ! e )                                                         {
    Information [ "ErrorCode"   ] = 30001                            ;
    Information [ "ErrorString" ] = QString ( "Execution failure"  ) ;
    r = false                                                        ;
  } else if ( ! r . valid ( ) )                                      {
    Information [ "ErrorCode"   ] = 30002                            ;
    Information [ "ErrorString" ] = QString ( "Invalid Parameters" ) ;
    r = false                                                        ;
    e = false                                                        ;
  }                                                                  ;
  return e                                                           ;
}

bool Server::HeaderCompleted(void)
{
  return header . parse ( pending ) ;
}

bool Server::ContentCompleted(void)
{
  if ( http . length ( ) <= 0 ) return false ;
  qint64 length = header . Length ( )        ;
  if ( length < 0             ) return false ;
  return ( pending . size ( ) >= length )    ;
}

void Server::BeforeResponse(Header & http)
{
  QDateTime DT = QDateTime::currentDateTime ( )                              ;
  http   . addItem ( "Stamp"    , DT.toString ("yyyy/MM/dd-hh:mm:ss.zzz") )  ;
  if ( Information . contains ( "Hostname" ) )                               {
    http . addItem ( "Hostname" , Information [ "Hostname" ] . toString() )  ;
  }                                                                          ;
}

Method * Server::FindMethod(Header & h)
{
  if ( NULL == methods ) return NULL ;
  return methods -> find ( "" , h )  ;
}

QString Server::Parse(QString & r,Value & p)
{
  int     offset     = 0                                        ;
  QString methodName = ""                                       ;
  ///////////////////////////////////////////////////////////////
  methodName = p . parseTag ( METHODNAME_TAG , r , &offset )    ;
  if ( methodName . size ( ) <= 0                   ) return "" ;
  ///////////////////////////////////////////////////////////////
  if ( ! p . nextTagIs ( PARAMS_TAG , r , &offset ) ) return "" ;
  if ( ! p . nextTagIs ( PARAM_TAG  , r , &offset ) ) return "" ;
  if ( ! p . fromXml ( r , &offset )                ) return "" ;
  if ( ! p . valid ( )                              ) return "" ;
  ///////////////////////////////////////////////////////////////
  return methodName                                             ;
}

QString Server::GenerateResponse(Header & p,QString & resultXml)
{
  QByteArray b                      ;
  QString    c                      ;
  QString    h                      ;
  ///////////////////////////////////
  c      = HeaderRESPONSE_1         ;
  c     += resultXml                ;
  c     += HeaderRESPONSE_2         ;
  b      = c . toUtf8 ( )           ;
  h      = GenerateHeader ( p , b ) ;
  h     += c                        ;
  return h                          ;
}

QString Server::FaultResponse(Header & p,QString errorMsg,int errorCode)
{
  Value      f                                 ;
  QByteArray b                                 ;
  QString    c                                 ;
  QString    h                                 ;
  //////////////////////////////////////////////
  p . addItem ( "Message" , "Fault" )          ;
  //////////////////////////////////////////////
  f [ FAULTCODE   ] = errorCode                ;
  f [ FAULTSTRING ] = errorMsg                 ;
  //////////////////////////////////////////////
  c                 = FaultRESPONSE_1          ;
  c                += f . toXml ( )            ;
  c                += FaultRESPONSE_2          ;
  b                 = c . toUtf8 ( )           ;
  h                 = GenerateHeader ( p , b ) ;
  h                += c                        ;
  return h                                     ;
}

QString Server::GenerateHeader(Header & p,QByteArray & b)
{
  QString h                                                        ;
  p . addItem          ( "Server" , QString::fromUtf8 ( XMLRPC ) ) ;
  p . addLength        ( b . size ( )                            ) ;
  p . addMime          ( "text/xml"                              ) ;
  h  = QString         ( "HTTP/1.1 200 OK\r\n"                   ) ;
  h += p . httpHeader  (                                         ) ;
  return  h                                                        ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
