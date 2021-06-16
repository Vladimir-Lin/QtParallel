/****************************************************************************
 *
 * Copyright (C) 2001~2016 Neutrino International Inc.
 *
 * Author   : Brian Lin ( Foxman , Vladimir Lin , Vladimir Forest )
 * E-mail   : lin.foxman@gmail.com
 *          : lin.vladimir@gmail.com
 *          : wolfram_lin@yahoo.com
 *          : wolfram_lin@sina.com
 *          : wolfram_lin@163.com
 * Skype    : wolfram_lin
 * WeChat   : +86-153-0271-7160 (Vladimir-Lin)
 * WhatsApp : +86-153-0271-7160
 * QQ       : lin.vladimir@gmail.com (2107437784)
 * URL      : http://QtParallel.sourceforge.net/
 *
 * QtParallel acts as an interface for Qt to do parallel computing.
 *
 * Copyright 2001 ~ 2016
 *
 ****************************************************************************/

#ifndef QT_PARALLEL_H
#define QT_PARALLEL_H

#include <QtCore>

#include <QtNetwork>
#include <QtUUID>
#include <QtZMQ>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_QTPARALLEL_LIB)
#    define Q_PARALLEL_EXPORT Q_DECL_EXPORT
#  else
#    define Q_PARALLEL_EXPORT Q_DECL_IMPORT
#  endif
#else
#    define Q_PARALLEL_EXPORT
#endif

#ifndef QT_PARALLEL_LIB
#define QT_PARALLEL_LIB 1
#endif

#ifndef NO_PARALLEL_NAMESPACE
#define QTPARALLEL_NAMESPACE               QtParallel
#define BEGIN_PARALLEL_NAMESPACE namespace QtParallel {
#define END_PARALLEL_NAMESPACE                        }
#define XmlValue                           QtParallel::Value
#define XmlHeader                          QtParallel::Header
#define XmlClient                          QtParallel::Client
#define XmlMethod                          QtParallel::Method
#else
#define QTPARALLEL_NAMESPACE
#define BEGIN_PARALLEL_NAMESPACE
#define END_PARALLEL_NAMESPACE
#define XmlValue                                       Value
#define XmlHeader                                      Header
#define XmlClient                                      Client
#define XmlMethod                                      Method
#endif

BEGIN_PARALLEL_NAMESPACE

class Q_PARALLEL_EXPORT Value           ;
class Q_PARALLEL_EXPORT Extras          ;
class Q_PARALLEL_EXPORT Header          ;
class Q_PARALLEL_EXPORT Companion       ;
class Q_PARALLEL_EXPORT Method          ;
class Q_PARALLEL_EXPORT Exists          ;
class Q_PARALLEL_EXPORT ChatMethod      ;
class Q_PARALLEL_EXPORT FileMethod      ;
class Q_PARALLEL_EXPORT CompanionMethod ;
class Q_PARALLEL_EXPORT Methods         ;
class Q_PARALLEL_EXPORT Talk            ;
class Q_PARALLEL_EXPORT Client          ;
class Q_PARALLEL_EXPORT Server          ;
class Q_PARALLEL_EXPORT Listener        ;
class Q_PARALLEL_EXPORT Neighbors       ;
class Q_PARALLEL_EXPORT Dispatcher      ;
#ifndef NO_SYSTEM_METHODS
class Q_PARALLEL_EXPORT Listing         ;
class Q_PARALLEL_EXPORT Help            ;
#endif

typedef enum            {
  CommNothing      =  0 , /*! Listener is not listening */
  CommIPv4         =  1 , /*! Listener is using TCP IPv4 as incoming connection */
  CommIPv6         =  2 , /*! Listener is using TCP IPv6 as incoming connection */
  CommZMQ          =  3 } /*! Listener is using ZMQ as incoming connection */
  CommunicationChannels ;

/*!
 * Value is a XML tagged container
 */
class Q_PARALLEL_EXPORT Value
{
  public:

    typedef enum        {
      TypeInvalid  =  0 ,
      TypeBoolean  =  1 ,
      TypeInt      =  2 ,
      TypeDouble   =  3 ,
      TypeString   =  4 ,
      TypeDateTime =  5 ,
      TypeBase64   =  6 ,
      TypeArray    =  7 ,
      TypeStruct   =  8 ,
      TypeTuid     =  9 ,
      TypeUuid     = 10 ,
      TypeEnd      = 11 }
      Type              ;

    typedef QByteArray                BinaryData  ;
    typedef QList < Value           > ValueArray  ;
    typedef QMap  < QString , Value > ValueStruct ;

  protected:

    Type        _type    ;
    bool        asBool   ;
    int         asInt    ;
    qint64      asTuid   ;
    quint64     asUuid   ;
    double      asDouble ;
    QString     asString ;
    QDateTime   asTime   ;
    BinaryData  asBinary ;
    ValueArray  asArray  ;
    ValueStruct asStruct ;

  public:

    explicit       Value        (void                              ) ;
    explicit       Value        (bool            value             ) ;
    explicit       Value        (int             value             ) ;
    explicit       Value        (qint64          value             ) ;
    explicit       Value        (quint64         value             ) ;
    explicit       Value        (double          value             ) ;
    explicit       Value        (QDateTime       dateTime          ) ;
    explicit       Value        (const QString & value             ) ;
    explicit       Value        (const char    * value             ) ;
    explicit       Value        (void          * value,int   nBytes) ;
    explicit       Value        (QString       & xml  ,int * offset) ;
                   Value        (const Value   & rhs               ) ;
    virtual       ~Value        (void                              ) ;

    virtual void   clear        (void) ;

    Value &        operator =   (const Value            & rhs) ;
    Value &        operator =   (const bool             & rhs) ;
    Value &        operator =   (const int              & rhs) ;
    Value &        operator =   (const qint64           & rhs) ;
    Value &        operator =   (const quint64          & rhs) ;
    Value &        operator =   (const double           & rhs) ;
    Value &        operator =   (const char             * rhs) ;
    Value &        operator =   (const QString          & rhs) ;
    Value &        operator =   (const QDateTime        & rhs) ;
    Value &        operator =   (QMap<QString,QVariant> & rhs) ;
    Value &        operator =   (const UUIDs            & rhs) ;

    bool           operator ==  (const Value & other) const ;
    bool           operator !=  (const Value & other) const ;

    const Type &   getType      (void) const ;
    bool           isType       (int type) ;

    operator bool        & ( ) { assertType ( TypeBoolean  ) ; return asBool   ; }
    operator int         & ( ) { assertType ( TypeInt      ) ; return asInt    ; }
    operator qint64      & ( ) { assertType ( TypeTuid     ) ; return asTuid   ; }
    operator quint64     & ( ) { assertType ( TypeUuid     ) ; return asUuid   ; }
    operator double      & ( ) { assertType ( TypeDouble   ) ; return asDouble ; }
    operator QString     & ( ) { assertType ( TypeString   ) ; return asString ; }
    operator QDateTime   & ( ) { assertType ( TypeDateTime ) ; return asTime   ; }
    operator BinaryData  & ( ) { assertType ( TypeBase64   ) ; return asBinary ; }

    const Value  & operator [ ] (int i) const ;
          Value  & operator [ ] (int i) ;

    Value        & operator [ ] (const QString & k) ;
    Value        & operator [ ] (const char    * k) ;

    bool           valid        (void) const ;
    bool           isBool       (void) const ;
    bool           isInt        (void) const ;
    bool           isTuid       (void) const ;
    bool           isUuid       (void) const ;
    bool           isDouble     (void) const ;
    bool           isString     (void) const ;
    bool           isTime       (void) const ;
    bool           isBinary     (void) const ;
    bool           isArray      (void) const ;
    bool           isStruct     (void) const ;

    int            size         (void) const ;
    void           setSize      (int size) ;

    QVariantList   toVariants   (void) ;
    QVariant       toVariant    (void) ;
    int            toMap        (QMap<QString,QVariant> & map) ;

    bool           hasMember    (const QString & name) const ;
    QStringList    Members      (void) ;

    bool           fromXml      (QString & valueXml,int * offset) ;
    QString        toXml        (void) ;

    QString        Encode       (QString & raw) ;
    QString        Decode       (QString & encoded) ;
    QString        getNextTag   (const QString & xml,int * offset) ;
    QString        parseTag     (const char    * tag,const QString & xml,int * offset) ;
    bool           findTag      (const char    * tag,const QString & xml,int * offset) ;
    bool           nextTagIs    (const char    * tag,const QString & xml,int * offset)  ;

  protected:

    void    invalidate          (void) ;

    void    assertType          (Type t) ;
    void    assertArray         (int size) ;
    void    assertStruct        (void) ;

    bool    boolFromXml         (QString & valueXml,int * offset) ;
    bool    intFromXml          (QString & valueXml,int * offset) ;
    bool    tuidFromXml         (QString & valueXml,int * offset) ;
    bool    uuidFromXml         (QString & valueXml,int * offset) ;
    bool    doubleFromXml       (QString & valueXml,int * offset) ;
    bool    stringFromXml       (QString & valueXml,int * offset) ;
    bool    timeFromXml         (QString & valueXml,int * offset) ;
    bool    binaryFromXml       (QString & valueXml,int * offset) ;
    bool    arrayFromXml        (QString & valueXml,int * offset) ;
    bool    structFromXml       (QString & valueXml,int * offset) ;

    QString boolToXml           (void) ;
    QString intToXml            (void) ;
    QString tuidToXml           (void) ;
    QString uuidToXml           (void) ;
    QString doubleToXml         (void) ;
    QString stringToXml         (void) ;
    QString timeToXml           (void) ;
    QString binaryToXml         (void) ;
    QString arrayToXml          (void) ;
    QString structToXml         (void) ;

  private:

} ;

/*!
 * Extras is used for transmit/receive data
 */
class Extras
{
  public:

    QMap<QString,QVariant> Information ;
    QFile                  Reader      ;
    QFile                  Writer      ;

    explicit     Extras       (void) ;
    virtual     ~Extras       (void) ;

    virtual bool recycle      (void) ;
    virtual bool destroy      (void) ;

    virtual bool transmitting (Header & params,QByteArray & data) ;
    virtual bool receiving    (Header & params,QByteArray & data) ;

  protected:

    void         Copy         (QStringList & keys,Header & params) ;

  private:

} ;

/*!
 * Header is a HTTP properties container
 */
class Q_PARALLEL_EXPORT Header
{
  public:

    QMap<QString,QVariant> Tags  ;
    QStringList            Items ;
    Extras               * Data  ;

    explicit         Header     (void) ;
    explicit         Header     (QByteArray   & header) ;
                     Header     (const Header & header) ;
    virtual         ~Header     (void) ;

    virtual void     clear      (void) ;
    virtual bool     parse      (QByteArray & header) ;
    virtual int      splitter   (QByteArray & header) ;
    virtual int      content    (QByteArray & header) ;
    virtual bool     addTag     (QString    & tag   ) ;
    virtual QString  compose    (QString tag,QString value) ;
    virtual int      addItem    (QString tag,QString value) ;
    virtual void     addLength  (qint64  length) ;
    virtual void     addMime    (QString mime  ) ;
    virtual QString  httpHeader (void) ;
    virtual bool     isInvoke   (QString invoke) ;
    virtual bool     isMime     (QString mime) ;
    virtual qint64   Length     (void) ;

    virtual Header & operator = (const Header & header) ;
    virtual Header & operator = (      Header & header) ;

  protected:

  private:

} ;

/*!
 * Companion is a basic sharing information between machines
 */
class Q_PARALLEL_EXPORT Companion
{
  public:

    QMap<QString,QVariant> Information ;

    explicit        Companion       (void) ;
    explicit        Companion       (bool self) ;
    virtual        ~Companion       (void) ;

    QString         name            (void) ;
    virtual bool    isSelf          (void) ;
    virtual bool    isOffline       (void) ;
    virtual void    destroyer       (void) ;

    virtual bool    lock            (void) ;
    virtual bool    unlock          (void) ;

    virtual bool    retrieve        (void) ;

    virtual bool    FromXml         (Value   & value) ;
    virtual bool    ToXml           (Value   & value) ;

    virtual bool    FromHTTP        (Header  & header) ;
    virtual bool    ToHTTP          (Header  & header) ;

    virtual bool    FromLine        (QString & message) ;
    virtual bool    ToLine          (QString & message) ;

    virtual int     Port            (void) ;
    virtual QString IpAddress       (void) ;
    virtual QString IpAddresses     (void) ;

    virtual int     setPort         (int port) ;
    virtual void    ObtainAddresses (QString hostname = "") ;

  protected:

    bool     self  ;

  private:

    QMutex * mutex ;

} ;

class Q_PARALLEL_EXPORT Method
{
  public:

    enum                {
      Execution   = 1   ,
      Transmitter = 2   ,
      Receiver    = 4 } ;

    explicit        Method       (void) ;
    explicit        Method       (const QString name            ,
                                  Methods     * method   = NULL ,
                                  qint64        instance = 0  ) ;
    virtual        ~Method       (void) ;

    virtual int     MethodType   (void) const ;
    virtual bool    isMethod     (QString name,Header & header) ;
    QString &       name         (void) ;
    qint64  &       instance     (void) ;
    virtual QString help         (void) ;
    virtual bool    join         (Methods & methods) ;
    virtual void    assign       (Methods * methods) ;

    virtual bool    capable      (int functions) ;
    virtual bool    decapitate   (void) ;

    virtual bool    execute      (Value  & params,Value      & result) ;
    virtual bool    transmitting (Header & params,QByteArray & data  ) ;
    virtual bool    receiving    (Header & params,QByteArray & data  ) ;

  protected:

    Methods * methods  ;

  private:

    QString   Name     ;
    qint64    Instance ;

} ;

#ifndef NO_SYSTEM_METHODS

class Q_PARALLEL_EXPORT Listing : public Method
{
  public:

    explicit        Listing      (Methods * method) ;
    virtual        ~Listing      (void) ;

    static  QString MethodName   (void) ;

    virtual int     MethodType   (void) const ;
    virtual QString help         (void) ;

    virtual bool    capable      (int functions) ;
    virtual bool    decapitate   (void) ;

    virtual bool    execute      (Value  & params,Value      & result) ;

  protected:

  private:

} ;

class Q_PARALLEL_EXPORT Help : public Method
{
  public:

    explicit        Help         (Methods * method) ;
    virtual        ~Help         (void) ;

    static  QString MethodName   (void) ;

    virtual int     MethodType   (void) const ;
    virtual QString help         (void) ;

    virtual bool    capable      (int functions) ;
    virtual bool    decapitate   (void) ;

    virtual bool    execute      (Value  & params,Value      & result) ;

  protected:

  private:

} ;

#endif

class Q_PARALLEL_EXPORT Exists : public Method
{
  public:

    explicit        Exists       (Methods * method) ;
    virtual        ~Exists       (void) ;

    static  QString MethodName   (void) ;

    virtual int     MethodType   (void) const ;
    virtual QString help         (void) ;

    virtual bool    capable      (int functions) ;
    virtual bool    decapitate   (void) ;

    virtual bool    execute      (Value  & params,Value      & result) ;

  protected:

  private:

} ;

class Q_PARALLEL_EXPORT ChatMethod : public Method
{
  public:

    QMap<QString,QVariant> Information ;

    explicit        ChatMethod   (QString name,Methods * method) ;
    virtual        ~ChatMethod   (void) ;

    virtual int     MethodType   (void) const ;
    virtual QString help         (void) ;

    virtual bool    capable      (int functions) ;
    virtual bool    decapitate   (void) ;

    virtual bool    execute      (Value  & params,Value      & result) ;
    virtual bool    transmitting (Header & params,QByteArray & data  ) ;
    virtual bool    receiving    (Header & params,QByteArray & data  ) ;

  protected:

  private:

} ;

class Q_PARALLEL_EXPORT FileMethod : public Method
{
  public:

    QMap<QString,QVariant> Information ;

    explicit        FileMethod   (QString name,Methods * method) ;
    virtual        ~FileMethod   (void) ;

    virtual int     MethodType   (void) const ;
    virtual QString help         (void) ;

    virtual bool    capable      (int functions) ;
    virtual bool    decapitate   (void) ;

    virtual bool    execute      (Value  & params,Value      & result) ;
    virtual bool    transmitting (Header & params,QByteArray & data  ) ;
    virtual bool    receiving    (Header & params,QByteArray & data  ) ;

  protected:

    virtual bool    System       (Value & result) ;
    virtual bool    Devices      (Value & result) ;
    virtual bool    GetFileInfo  (Value  & params,Value & result) ;
    virtual bool    GetDirectory (Value  & params,Value & result) ;
    virtual bool    FileActions  (Value  & params,Value & result) ;

  private:

} ;

class Q_PARALLEL_EXPORT CompanionMethod : public Companion
                                        , public Method
{
  public:

    explicit        CompanionMethod (QString companion,Methods * method) ;
    virtual        ~CompanionMethod (void) ;

    static  QString MethodName      (void) ;

    virtual void    destroyer       (void) ;

    virtual int     MethodType      (void) const ;
    virtual bool    isMethod        (QString name,Header & header) ;
    virtual QString help            (void) ;

    virtual bool    capable         (int functions) ;
    virtual bool    decapitate      (void) ;

    virtual bool    execute         (Value  & params,Value      & result) ;
    virtual bool    transmitting    (Header & params,QByteArray & data  ) ;
    virtual bool    receiving       (Header & params,QByteArray & data  ) ;

  protected:

  private:

} ;

class Q_PARALLEL_EXPORT Methods
{
  public:

    explicit         Methods     (void) ;
    virtual         ~Methods     (void) ;

    virtual void     clear       (void) ;
    virtual void     clean       (void) ;

    virtual bool     join        (Method * method) ;
    virtual bool     remove      (Method * method) ;

    virtual Method * find        (QString name,Header & header) ;
    virtual Method * find        (QString name      ) ;
    virtual Method * find        (qint64  instance  ) ;
    virtual Method * findType    (int     methodType) ;

    virtual bool     Listings    (Value & result) ;

    virtual void     MethodError (int level,int code) ;

  protected:

    QList<Method *> methods ;

  private:

    QMutex       *  mutex   ;

} ;

class Q_PARALLEL_EXPORT Talk
{
  public:

    SUID                   uuid        ;
    bool                 * Controller  ;
    QMap<QString,QVariant> Information ;

    explicit       Talk          (void) ;
    virtual       ~Talk          (void) ;

    int &          CurrentState  (void) ;

    virtual void   destroyer       (void) ;

    virtual bool   Lookup        (void) ;
    virtual bool   ConnectTo     (void) ;
    virtual bool   Accepted      (void * packet) ;

    virtual qint64 IdleTime      (void) ;
    virtual bool   isTimeout     (qint64   msec) ;
    virtual bool   isTimeout     (QVariant msec) ;
    virtual bool   isConnected   (void) ;
    virtual void   TeaTime       (int action,int step) ;

    virtual bool   Close         (void) ;
    virtual int    Receive       (QByteArray & data,int utimeout = 100000) ;
    virtual int    Transmit      (QByteArray & data,int utimeout = 100000) ;

    virtual bool   PourIn        (void) ;
    virtual bool   ToPeer        (void) ;

  protected:

    int        state      ;
    int        port       ;
    QByteArray ReadBuffer ;
    QByteArray SendBuffer ;
    QByteArray pending    ;
    void     * comm       ;

    bool           isContinue    (void) ;

    bool           nonFatalError (void) ;
    int            getError      (void) ;
    QString        getErrorMsg   (void) ;
    QString        getErrorMsg   (int error) ;

    virtual void   TalkError     (int level,int code) ;

    virtual bool   LookupTcp     (void) ;
    virtual bool   ConnectTcp    (void) ;
    virtual bool   CloseTcp      (void) ;
    virtual bool   TcpWorking    (qint64 msec) ;
    virtual int    ReceiveTcp    (QByteArray & data,int utimeout = 100000) ;
    virtual int    TransmitTcp   (QByteArray & data,int utimeout = 100000) ;

  private:

} ;

class Q_PARALLEL_EXPORT Client : public Talk
{
  public:

    typedef enum       {
      NoConnection = 0 ,
      Connecting   = 1 ,
      Connected    = 2 ,
      Idle         = 3 ,
      WriteRequest = 4 ,
      ReadHeader   = 5 ,
      ReceiveData  = 6 ,
      TransmitData = 7 ,
      ReadResponse = 8 }
      ClientState      ;

    /*! These variables are for automated `execute` function */
    int          ExecuteID    ; /*! assign the Function ID for execute */
    Value        Parameters   ;
    Value        Results      ;
    Header       Http         ;
    QString      UseMethod    ;
    qint64       UseInstance  ;
    bool         ReturnBool   ;
    QByteArray * ExternalData ;
    QIODevice  * ExternalIO   ;

    explicit         Client               (void) ;
    explicit         Client               (int          commType            ) ;
    explicit         Client               (const char * host                  ,
                                           int          port                  ,
                                           const char * uri      = NULL       ,
                                           int          commType = CommIPv4 ) ;
    explicit         Client               (QString      host                  ,
                                           int          port                  ,
                                           QString      uri = QString ( )     ,
                                           int          commType = CommIPv4 ) ;
    virtual         ~Client               (void) ;

    virtual int      setPort              (int port) ;
    virtual bool     setHostname          (QString host) ;
    virtual bool     setURI               (QString uri) ;

    virtual void     destroyer            (void) ;

    virtual Header & HttpHeader           (void) ;
    virtual bool     setCommunication     (int commType) ;
    virtual bool     RemoveComm           (void) ;

    virtual bool     EnsureConnected      (void) ;
    virtual bool     isRunning            (void) ;
    virtual bool     execute              (void) ;

    virtual bool     executeInstance      (void) ; /*! Function ID = 1 */
    virtual bool     execute              (qint64        instance ,
                                           Value       & params   ,
                                           Value       & result ) ;
    virtual bool     executeMethod        (void) ; /*! Function ID = 0 */
    virtual bool     execute              (QString       method   ,
                                           Value       & params   ,
                                           Value       & result ) ;
    virtual bool     execute              (const char  * method   ,
                                           Value       & params   ,
                                           Value       & result ) ;

    virtual bool     transmitInstanceData (void) ; /*! Function ID = 3 */
    virtual bool     transmitting         (qint64        instance ,
                                           Header      & params   ,
                                           QByteArray  & data   ) ;
    virtual bool     transmitMethodData   (void) ; /*! Function ID = 2 */
    virtual bool     transmitting         (QString       method   ,
                                           Header      & params   ,
                                           QByteArray  & data   ) ;
    virtual bool     transmitting         (const char  * method   ,
                                           Header      & params   ,
                                           QByteArray  & data   ) ;

    virtual bool     transmitInstanceIO   (void) ; /*! Function ID = 5 */
    virtual bool     transmitting         (qint64        instance ,
                                           Header      & params   ,
                                           QIODevice   & io     ) ;
    virtual bool     transmitMethodIO     (void) ; /*! Function ID = 4 */
    virtual bool     transmitting         (QString       method   ,
                                           Header      & params   ,
                                           QIODevice   & io     ) ;
    virtual bool     transmitting         (const char  * method   ,
                                           Header      & params   ,
                                           QIODevice   & io     ) ;

    virtual bool     receiveInstanceData  (void) ; /*! Function ID = 7 */
    virtual bool     receiving            (qint64        instance ,
                                           Header      & params   ,
                                           QByteArray  & data   ) ;
    virtual bool     receiveMethodData    (void) ; /*! Function ID = 6 */
    virtual bool     receiving            (QString       method   ,
                                           Header      & params   ,
                                           QByteArray  & data   ) ;
    virtual bool     receiving            (const char  * method   ,
                                           Header      & params   ,
                                           QByteArray  & data   ) ;

    virtual bool     receiveInstanceIO    (void) ; /*! Function ID = 9 */
    virtual bool     receiving            (qint64        instance ,
                                           Header      & params   ,
                                           QIODevice   & io     ) ;
    virtual bool     receiveMethodIO      (void) ; /*! Function ID = 8 */
    virtual bool     receiving            (QString       method   ,
                                           Header      & params   ,
                                           QIODevice   & io     ) ;
    virtual bool     receiving            (const char  * method   ,
                                           Header      & params   ,
                                           QIODevice   & io     ) ;

  protected:

    Header     header    ;
    QString    http      ;
    QString    request   ;
    QString    response  ;
    bool       executing ;

    virtual void     TalkError            (int level,int code) ;
    virtual void     ClientError          (int level,int code) ;

    virtual bool     TryClose             (void) ;

    virtual bool     execute              (Value & result) ;
    virtual bool     transmitting         (Header      & params   ,
                                           QByteArray  & data   ) ;
    virtual bool     transmitting         (Header      & params   ,
                                           QIODevice   & io     ) ;
    virtual bool     receiving            (Header      & params   ,
                                           QByteArray  & data   ) ;
    virtual bool     receiving            (Header      & params   ,
                                           QIODevice   & io     ) ;

    bool             HeaderCompleted      (void) ;
    bool             ContentCompleted     (void) ;

    bool             ParseResponse        (QString & response  ,Value & result) ;
    QString          GenerateHeader       (Header  & header    ,qint64  size  ) ;
    QString          CreateRequest        (QString   methodName,Value & params) ;
    QString          CreateRequest        (qint64    instance  ,Value & params) ;
    QString          GenerateRequest      (QString   methodName,Value & params) ;

    virtual void     PrepareTcp           (void) ;

  private:

} ;

class Q_PARALLEL_EXPORT Server : public Talk
{
  public:

    typedef enum        {
      NoConnection   = 0 ,
      Idle           = 1 ,
      ReadHeader     = 2 ,
      ReadRequest    = 3 ,
      ReceiveData    = 4 ,
      TransmitData   = 5 ,
      ExecuteRequest = 6 ,
      WriteResponse  = 7 }
      ServerState        ;

    explicit         Server           (void) ;
    explicit         Server           (Methods * methods,void * incoming = NULL) ;
    virtual         ~Server           (void) ;

    virtual void     setMethods       (Methods * methods ) ;
    virtual bool     Accept           (void    * incoming) ;

    virtual bool     Prepare          (void) ;
    virtual bool     Watching         (void) ;

    virtual void     destroyer        (void) ;
    virtual bool     RemoveComm       (void) ;

  protected:

    Methods  * methods  ;
    Header     header   ;
    QString    http     ;
    QString    request  ;
    QString    response ;

    virtual void     BeforeResponse   (Header & http) ;
    virtual Method * FindMethod       (Header & http) ;
    virtual bool     NormalWatch      (void) ;

    virtual bool     PrepareTcp       (void) ;
    virtual bool     WatchTcp         (void) ;

    virtual void     TalkError        (int level,int code) ;
    virtual void     ServerError      (int level,int code) ;

    virtual bool     execute          (QString  methodName ,
                                       Header & header     ,
                                       Value  & params     ,
                                       Value  & result   ) ;
    virtual bool     execute          (qint64   instance   ,
                                       Value  & params     ,
                                       Value  & result   ) ;

    bool             HeaderCompleted  (void) ;
    bool             ContentCompleted (void) ;
    QString          Parse            (QString & request,Value      & params   ) ;
    QString          GenerateResponse (Header  & http   ,QString    & resultXml) ;
    QString          GenerateHeader   (Header  & http   ,QByteArray & body     ) ;
    QString          FaultResponse    (Header  & http   ,QString errorMsg,int errorCode) ;

  private:

} ;

/*!
 * Listener is the incoming connections monitor
 * It listens and accepts the outside connections
 */

class Q_PARALLEL_EXPORT Listener
{
  public:

    explicit     Listener            (void) ;
    virtual     ~Listener            (void) ;

    virtual bool Hosting             (void) ; /*! Persistently bind and listening */
    virtual bool BindListener        (void) ; /*! Bind local address */
    virtual bool Incoming            (void) ; /*! Monitor incoming connections */
    virtual bool CloseListener       (void) ; /*! Complete clean up listener */

  protected:

    int    tcpPort   ; /*! In TCP, IPv4 and IPv6 mode , this is the listening port */
    void * Listening ; /*! Listening data packet in all modes */

    virtual bool AbortListening      (void) ; /*! AbortListening controls the process to abort internal loop */
    virtual bool ListenerGaps        (void) ; /*! ListenerGaps is the interval between each listening action */

    virtual bool Accepting           (void * incoming) ; /*! Accept a new connection */

    virtual int  await               (void) ; /*! milliseconds for TCP select waiting time */
    virtual int  backlog             (void) ; /*! TCP listen function backlog parameter */

    virtual bool BindTcp             (void) ;
    virtual bool WatchTcp            (void) ;
    virtual bool CloseTcpListener    (void) ;

    virtual void ListenerError       (int level,int code) ;

    virtual int  ListenReceiveBuffer (void) ;
    virtual int  ListenSendingBuffer (void) ;

    /*!
     * Error code
     *
     * 10001 - obtaining a socket failure
     * 10002 - set non-block socket failure
     * 10003 - set reuse address failure
     * 10004 - bind local address failure
     * 10005 - listen a socket failure
     * 10006 - set UDP socket to broadcast failure
     * 20001 - select a socket failure
     * 20002 - socket has an exception
     * 21001 - accept a socket failure
     * 21002 - set accepted socket to non-block failure
     * 21003 - failure to create a new connection for incoming connection
     *
     */

  private:

} ;

/*!
 * class Neighbors emits broadcast signals and bind on udp socket port
 * to find out who are in our network neighborhood
 *
 * There are three methods in Neighbors:
 *
 * 1. Probe
 * 2. Bind for waiting
 * 3. Broadcast for notify the other neighbors
 */
class Q_PARALLEL_EXPORT Neighbors
{
  public:

    enum                  {
      UdpOperation  = 0   ,
      ProbeNeighbor = 1   ,
      WaitNeighbor  = 2   ,
      FindNeighbor  = 3 } ;

    explicit     Neighbors          (void) ;
    virtual     ~Neighbors          (void) ;

    virtual bool ProbeNeighbors     (int state = 101) ;
    virtual bool WaitNeighbors      (void) ;
    virtual bool FindNeighbors      (void) ;

  protected:

    int udpPort ;

    virtual bool AbortNeighbors     (int method) ;
    virtual bool NeighborClosed     (int method) ;
    virtual bool NeighborGaps       (int method) ;

    virtual int  NeighborBufferSize (void) ;
    virtual int  ReceiveBufferSize  (void) ;
    virtual int  SendingBufferSize  (void) ;

    virtual int  NoteTo             (int method,QByteArray & data) ;
    virtual int  NoteFrom           (int method,QByteArray & data) ;

    virtual bool ProbeUdp           (int state) ;
    virtual bool DetectMyself       (void) ;

    virtual bool BindUdp            (void) ;
    virtual bool BroadcastUdp       (void) ;

    virtual void NeighborError      (int level,int code) ;

  private:

    int          BroadcastSocket    (void) ;
    void         setNonblock        (int socket) ;
    bool         uselect            (int socket,long long usec) ;
    void         AnyAddress         (struct sockaddr_in & a,int port) ;
    void         BroadcastAddress   (struct sockaddr_in & a,int port) ;
    void         CloseUdp           (int socket) ;

} ;

/*!
 * Dispatcher is an integration of all required distributed computation nodes
 */
class Q_PARALLEL_EXPORT Dispatcher : public Methods
                                   , public Listener
                                   , public Neighbors
{
  public:

    QMap < QString , QVariant > Information ;
    QMap < int     , QString  > ErrorString ;

    explicit            Dispatcher         (void) ;
    virtual            ~Dispatcher         (void) ;

    static  bool        Initialize         (void) ;
    static  bool        Recovery           (void) ;

    virtual void        Purge              (void) ;
    virtual void        WaitThreads        (void) ;

    virtual void        setPort            (int port) ;
    virtual int         Threads            (void) ;

    virtual bool        exists             (void) ;
    virtual bool        exists             (QString ipaddress,QString host,int port = -1) ;

    virtual void        DispatcherLock     (QString name) ;
    virtual void        DispatcherUnlock   (QString name) ;

    virtual void        setGift            (QString      gift) ;
    virtual void        setGift            (QByteArray & gift) ;

    virtual bool        hasCompanion       (QString name) ;
    virtual int         Offline            (QString name) ;
    virtual Companion * FindCompanion      (QString name) ;
    virtual int         Join               (Companion * companion) ;
    virtual QStringList Others             (void) ;

    virtual void        MethodError        (int level,int code) ;

    virtual void        DetectByUdp        (void) ;
    virtual void        ProbeByUdp         (void) ;
    virtual void        WatchCompanions    (void) ;

    virtual SUID        AddClient          (Client * client) ;
    virtual Client *    FindClient         (SUID uuid) ;
    virtual bool        ExecuteClient      (SUID uuid) ;
    virtual bool        RemoveClient       (SUID uuid) ;

  protected:

    QList<Companion *> companions        ;
    QList<Companion *> deletedCompanions ;
    QList<Server    *> servers           ;
    QList<Server    *> deletedServers    ;
    QList<Client    *> clients           ;
    Companion       *  ParseCompanion    ;
    QByteArray         NeighborGift      ;
    QByteArray         Knock             ;

    virtual bool        Cancellation       (void) ;

    /*! Inherits Listener functions */

    virtual void        RunListener        (void) ;
    virtual bool        AbortListening     (void) ;
    virtual bool        ListenerGaps       (void) ;

    virtual bool        Accepting          (void * incoming) ;

    virtual int         await              (void) ;
    virtual int         backlog            (void) ;

    virtual Server    * NewServer          (void           ) ; /*! allocate a new server */
    virtual Server    * FindServer         (SUID     uuid  ) ; /*! looking for server pointer by uuid */
    virtual int         JoinServer         (Server * server) ; /*! server joins lists */
    virtual int         RemoveServer       (Server * server) ; /*! remove a server connection from lists */
    virtual bool        StartServer        (SUID     uuid  ) ; /*! a virtual call to start a server,
                                                                   normally you should inherit this
                                                                   function to start a thread */
    virtual bool        RunServer          (SUID     uuid  ) ; /*! thread function should call this
                                                                   function to actually run a server
                                                                   connection */

    /*! Inherits Neighbors functions */

    virtual bool        AbortNeighbors     (int method) ;
    virtual bool        NeighborClosed     (int method) ;
    virtual bool        NeighborGaps       (int method) ;

    virtual int         NeighborBufferSize (void) ;
    virtual int         ReceiveBufferSize  (void) ;
    virtual int         SendingBufferSize  (void) ;
    virtual int         ListenReceiveBuffer (void) ;
    virtual int         ListenSendingBuffer (void) ;

    virtual int         NoteTo             (int method,QByteArray & data) ;
    virtual int         NoteFrom           (int method,QByteArray & data) ;

    virtual int         HasCompanion       (QByteArray & line) ;
    virtual int         LineCompanion      (QByteArray & line) ;
    virtual void        CompanionMessage   (QString    & message) ;

    virtual bool        DetectMyself       (void) ;

    virtual Companion * NewCompanion       (void) ;
    virtual Companion * NewCompanion       (QString name) ;

    virtual void        NeighborError      (int level,int code) ;
    virtual void        ListenerError      (int level,int code) ;
    virtual void        ReportError        (int level,int code) ;

    virtual bool        NotifyNeighbor     (QString      myself      ,
                                            QString      neighbor  ) ;
    virtual bool        Notify             (QString      myself      ,
                                            QString      neighbor    ,
                                            QVariantList arguments ) ;

    /*! Utilities */

    virtual void        PushThread         (void) ;
    virtual void        PopThread          (void) ;

  private:

    QMap < QString , QMutex *> LocalMutexz ;

} ;

END_PARALLEL_NAMESPACE

Q_PARALLEL_EXPORT void RegisterParallels (void) ;

Q_DECLARE_METATYPE(QtParallel::Value)
Q_DECLARE_METATYPE(QtParallel::Companion)
Q_DECLARE_METATYPE(QtParallel::Method)
Q_DECLARE_METATYPE(QtParallel::Methods)
Q_DECLARE_METATYPE(QtParallel::Talk)
Q_DECLARE_METATYPE(QtParallel::Client)
Q_DECLARE_METATYPE(QtParallel::Server)
Q_DECLARE_METATYPE(QtParallel::Listener)
Q_DECLARE_METATYPE(QtParallel::Neighbors)
Q_DECLARE_METATYPE(QtParallel::Dispatcher)

QT_END_NAMESPACE

#endif
