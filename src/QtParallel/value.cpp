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

#define DBLFMT "%.12f"
#define AMP    QChar('&')

static const char   rawEntity   [ ] = {   '<',   '>',    '&',    '\'',    '\"', 0 } ;
static const char * xmlEntity   [ ] = { "lt;", "gt;", "amp;", "apos;", "quot;", 0 } ;
static const int    xmlEntLen   [ ] = {     3,     3,      4,       5,       5, 0 } ;

static const char VALUE_TAG     [ ] = "<value>"     ;
static const char VALUE_ETAG    [ ] = "</value>"    ;

static const char BOOLEAN_TAG   [ ] = "<boolean>"   ;
static const char BOOLEAN_ETAG  [ ] = "</boolean>"  ;
static const char DOUBLE_TAG    [ ] = "<double>"    ;
static const char DOUBLE_ETAG   [ ] = "</double>"   ;
static const char INT_TAG       [ ] = "<int>"       ;
static const char INT_ETAG      [ ] = "</int>"      ;
static const char I4_TAG        [ ] = "<i4>"        ;
static const char I4_ETAG       [ ] = "</i4>"       ;
static const char I8_TAG        [ ] = "<i8>"        ;
static const char I8_ETAG       [ ] = "</i8>"       ;
static const char U8_TAG        [ ] = "<u8>"        ;
static const char U8_ETAG       [ ] = "</u8>"       ;
static const char STRING_TAG    [ ] = "<string>"    ;
static const char STRING_ETAG   [ ] = "</string>"   ;
static const char DATETIME_TAG  [ ] = "<datetime>"  ;
static const char DATETIME_ETAG [ ] = "</datetime>" ;
static const char BASE64_TAG    [ ] = "<base64>"    ;
static const char BASE64_ETAG   [ ] = "</base64>"   ;

static const char ARRAY_TAG     [ ] = "<array>"     ;
static const char DATA_TAG      [ ] = "<data>"      ;
static const char DATA_ETAG     [ ] = "</data>"     ;
static const char ARRAY_ETAG    [ ] = "</array>"    ;

static const char STRUCT_TAG    [ ] = "<struct>"    ;
static const char MEMBER_TAG    [ ] = "<member>"    ;
static const char NAME_TAG      [ ] = "<name>"      ;
static const char NAME_ETAG     [ ] = "</name>"     ;
static const char MEMBER_ETAG   [ ] = "</member>"   ;
static const char STRUCT_ETAG   [ ] = "</struct>"   ;

Value:: Value  ( void        )
      : _type  ( TypeInvalid )
      , asUuid ( 0           )
{
}

Value:: Value  ( bool value  )
      : _type  ( TypeBoolean )
      , asBool (      value  )
{
}

Value:: Value ( int value )
      : _type ( TypeInt   )
      , asInt (     value )
{
}

Value:: Value  ( qint64 v )
      : _type  ( TypeTuid )
      , asTuid (        v )
{
}

Value:: Value  ( quint64 v )
      : _type  ( TypeUuid  )
      , asUuid (         v )
{
}

Value:: Value  ( QDateTime dateTime )
      : _type  ( TypeDateTime       )
      , asTime (           dateTime )
{
}

Value:: Value    ( double v   )
      : _type    ( TypeDouble )
      , asDouble (        v   )
{
}

Value:: Value    ( const QString & v )
      : _type    ( TypeString        )
      , asString (                 v )
{
}

Value:: Value ( const char * v )
      : _type ( TypeString     )
{
  asString = ""                      ;
  if ( NULL == v ) return            ;
  asString = QString::fromUtf8 ( v ) ;
}

Value:: Value ( void * v , int nBytes )
      : _type ( TypeBase64            )
{
  asBinary . clear  (                           ) ;
  asBinary . append ( (const char *) v , nBytes ) ;
}

Value:: Value ( QString & xml,int * offset )
      : _type ( TypeInvalid                )
{
  if ( ! fromXml ( xml , offset ) ) _type = TypeInvalid ;
}

Value:: Value ( const Value & rhs )
      : _type ( TypeInvalid       )
{
  (*this) = rhs ;
}

Value::~Value (void)
{
  clear ( ) ;
}

void Value::clear(void)
{
  invalidate ( ) ;
}

const Value & Value::operator [ ] (int i) const
{
  if ( TypeArray != _type       ) return (*this) ;
  if ( asArray . count ( ) >= i ) return (*this) ;
  return asArray [ i ]                           ;
}

Value & Value::operator [ ] (int i)
{
  if ( i >= asArray . count ( ) ) {
    assertArray ( i + 1 )         ;
  }                               ;
  return asArray [ i ]            ;
}

Value & Value::operator [ ] (const QString & k)
{
  assertStruct ( )                          ;
  if ( _type != TypeStruct ) return (*this) ;
  return asStruct [ k ]                     ;
}

Value & Value::operator [ ] (const char * k)
{
  assertStruct (   )                        ;
  if ( _type != TypeStruct ) return (*this) ;
  if ( NULL  == k          ) return (*this) ;
  QString s = QString::fromUtf8 ( k )       ;
  return asStruct [ s ]                     ;
}

bool Value::valid(void) const
{
  return ( _type != TypeInvalid ) ;
}

bool Value::isBool(void) const
{
  return ( _type == TypeBoolean ) ;
}

bool Value::isInt(void) const
{
  return ( _type == TypeInt ) ;
}

bool Value::isTuid(void) const
{
  return ( _type == TypeTuid ) ;
}

bool Value::isUuid(void) const
{
  return ( _type == TypeUuid ) ;
}

bool Value::isDouble(void) const
{
  return ( _type == TypeDouble ) ;
}

bool Value::isString(void) const
{
  return ( _type == TypeString ) ;
}

bool Value::isTime(void) const
{
  return ( _type == TypeDateTime ) ;
}

bool Value::isBinary(void) const
{
  return ( _type == TypeBase64 ) ;
}

bool Value::isArray(void) const
{
  return ( _type == TypeArray ) ;
}

bool Value::isStruct(void) const
{
  return ( _type == TypeStruct ) ;
}

const Value::Type & Value::getType (void) const
{
  return _type ;
}

bool Value::isType(int type)
{
  return ( _type == (Type) type ) ;
}

void Value::setSize(int size)
{
  assertArray ( size ) ;
}

void Value::invalidate (void)
{
  _type    = TypeInvalid                    ;
  asBool   = false                          ;
  asInt    = 0                              ;
  asTuid   = 0                              ;
  asUuid   = 0                              ;
  asDouble = 0                              ;
  asString = ""                             ;
  asTime   = QDateTime::currentDateTime ( ) ;
  asBinary . clear                      ( ) ;
  asArray  . clear                      ( ) ;
  asStruct . clear                      ( ) ;
}

void Value::assertType(Value::Type t)
{
  if ( _type != TypeInvalid ) return ;
  _type = t                          ;
}

void Value::assertArray(int size)
{
  if ( _type == TypeInvalid )          {
    _type = TypeArray                  ;
    asArray . clear ( )                ;
  }                                    ;
  //////////////////////////////////////
  if ( _type != TypeArray   ) return   ;
  Value XRV                            ;
  //////////////////////////////////////
  if ( size <  0 ) return              ;
  //////////////////////////////////////
  if ( size == 0 )                     {
    asArray . clear ( )                ;
    return                             ;
  }                                    ;
  //////////////////////////////////////
  while ( asArray . count ( ) < size ) {
    asArray . append ( XRV )           ;
  }                                    ;
  //////////////////////////////////////
  while ( asArray . count ( ) > size ) {
    asArray . takeLast ( )             ;
  }                                    ;
}

void Value::assertStruct(void)
{
  if ( _type != TypeInvalid ) return ;
  _type = TypeStruct                 ;
}

Value & Value::operator = (QMap<QString,QVariant> & rhs)
{
  QStringList K = rhs . keys ( )           ;
  QString     k                            ;
  QString     s                            ;
  invalidate ( )                           ;
  _type = TypeStruct                       ;
  if ( K . count ( ) <= 0 ) return (*this) ;
  foreach ( k , K )                        {
    s = rhs [ k ] . toString ( )           ;
    asStruct [ k ] = s                     ;
  }                                        ;
  return (*this)                           ;
}

Value & Value::operator = (const UUIDs & U)
{
  SUID u                   ;
  invalidate ( )           ;
  _type   = TypeArray      ;
  asArray . clear ( )      ;
  foreach ( u , U )        {
    asArray << Value ( u ) ;
  }                        ;
  return (*this)           ;
}

Value & Value::operator = (const Value & rhs)
{
  if ( this == &rhs ) return (*this)                      ;
  invalidate ( )                                          ;
  _type = rhs . _type                                     ;
  switch ( _type )                                        {
    case TypeBoolean  : asBool   = rhs . asBool   ; break ;
    case TypeInt      : asInt    = rhs . asInt    ; break ;
    case TypeTuid     : asTuid   = rhs . asTuid   ; break ;
    case TypeUuid     : asUuid   = rhs . asUuid   ; break ;
    case TypeDouble   : asDouble = rhs . asDouble ; break ;
    case TypeDateTime : asTime   = rhs . asTime   ; break ;
    case TypeString   : asString = rhs . asString ; break ;
    case TypeBase64   : asBinary = rhs . asBinary ; break ;
    case TypeArray    : asArray  = rhs . asArray  ; break ;
    case TypeStruct   : asStruct = rhs . asStruct ; break ;
    default           : asUuid   = 0              ; break ;
  }                                                       ;
  return (*this)                                          ;
}

Value & Value::operator = (const bool & rhs)
{
  invalidate ( )       ;
  _type  = TypeBoolean ;
  asBool = rhs         ;
  return (*this)       ;
}

Value & Value::operator = (const int & rhs)
{
  invalidate ( )  ;
  _type = TypeInt ;
  asInt = rhs     ;
  return (*this)  ;
}

Value & Value::operator = (const qint64 & rhs)
{
  invalidate ( )    ;
  _type  = TypeTuid ;
  asTuid = rhs      ;
  return (*this)    ;
}

Value & Value::operator = (const quint64 & rhs)
{
  invalidate ( )    ;
  _type  = TypeUuid ;
  asUuid = rhs      ;
  return (*this)    ;
}

Value & Value::operator = (const double & rhs)
{
  invalidate ( )        ;
  _type    = TypeDouble ;
  asDouble = rhs        ;
  return (*this)        ;
}

Value & Value::operator = (const char * rhs)
{
  invalidate ( )                         ;
  _type      = TypeString                ;
  if ( NULL == rhs )                     {
    asString = ""                        ;
  } else                                 {
    asString = QString::fromUtf8 ( rhs ) ;
  }                                      ;
  return (*this)                         ;
}

Value & Value::operator = (const QString & rhs)
{
  invalidate ( )        ;
  _type    = TypeString ;
  asString = rhs        ;
  return (*this)        ;
}

Value & Value::operator = (const QDateTime & rhs)
{
  invalidate ( )        ;
  _type  = TypeDateTime ;
  asTime = rhs          ;
  return (*this)        ;
}

bool Value::operator == (const Value & other) const
{
  if (_type != other . _type ) return false                            ;
  switch ( _type )                                                     {
    case TypeBoolean  : return asBool   == other . asBool              ;
    case TypeInt      : return asInt    == other . asInt               ;
    case TypeTuid     : return asTuid   == other . asTuid              ;
    case TypeUuid     : return asUuid   == other . asUuid              ;
    case TypeDouble   : return asDouble == other . asDouble            ;
    case TypeDateTime : return asTime   == other . asTime              ;
    case TypeString   : return asString == other . asString            ;
    case TypeBase64   : return asBinary == other . asBinary            ;
    case TypeArray    : return asArray  == other . asArray             ;
    case TypeStruct   :                                                {
      if ( asStruct . count ( ) == other . asStruct . count ( ) )      {
        QStringList SSL =         asStruct . keys ( )                  ;
        QStringList SSO = other . asStruct . keys ( )                  ;
        if ( SSL . count ( ) != SSO . count ( ) ) return false         ;
        if (SSL        !=SSO        ) return false                     ;
        for (int i = 0 ; i < SSL . count ( ) ; i++ )                   {
          QString S = SSL [ i ]                                        ;
          if ( asStruct [ S ] != other . asStruct [ S ] ) return false ;
        }                                                              ;
      } else return false                                              ;
      return true                                                      ;
    }                                                                  ;
    default: break                                                     ;
  }                                                                    ;
  return true                                                          ;
}

bool Value::operator != (const Value & other) const
{
  return ! ( (*this) == other ) ;
}

int Value::size(void) const
{
  switch                         ( _type                 ) {
    case TypeString : return int ( asString . length ( ) ) ;
    case TypeBase64 : return int ( asBinary . size   ( ) ) ;
    case TypeArray  : return int ( asArray  . count  ( ) ) ;
    case TypeStruct : return int ( asStruct . count  ( ) ) ;
    default: break                                         ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  return 0                                                 ;
}

QVariant Value::toVariant(void)
{
  QVariant v                    ;
  switch           ( _type    ) {
    case TypeBoolean            :
      v = QVariant ( asBool   ) ;
    break                       ;
    case TypeInt                :
      v = QVariant ( asInt    ) ;
    break                       ;
    case TypeDouble             :
      v = QVariant ( asDouble ) ;
    break                       ;
    case TypeString             :
      v = QVariant ( asString ) ;
    break                       ;
    case TypeDateTime           :
      v = QVariant ( asTime   ) ;
    break                       ;
    case TypeBase64             :
      v = QVariant ( asBinary ) ;
    break                       ;
    case TypeTuid               :
      v = QVariant ( asTuid   ) ;
    break                       ;
    case TypeUuid               :
      v = QVariant ( asUuid   ) ;
    break                       ;
  }                             ;
  return v                      ;
}

QVariantList Value::toVariants(void)
{
  QVariantList v                                   ;
  if ( TypeArray != _type ) return v               ;
  for (int i = 0 ; i < asArray . count ( ) ; i++ ) {
    v << asArray [ i ] . toVariant ( )             ;
  }                                                ;
  return v                                         ;
}

int Value::toMap(QMap<QString,QVariant> & map)
{
  if ( _type != TypeStruct ) return map . count ( ) ;
  QStringList K = asStruct . keys ( )               ;
  QString     k                                     ;
  foreach ( k , K )                                 {
    map [ k ] = asStruct [ k ] . toVariant ( )      ;
  }                                                 ;
  return map . count ( )                            ;
}

bool Value::hasMember(const QString & name) const
{
  if ( _type != TypeStruct ) return false ;
  return asStruct . contains ( name )     ;
}

QStringList Value::Members(void)
{
  QStringList L                       ;
  if ( _type != TypeStruct ) return L ;
  L = asStruct . keys ( )             ;
  return L                            ;
}

bool Value::fromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset     ) return false                                    ;
  int xmlen = valueXml . length ( )                                         ;
  if ( (*offset) >= xmlen ) return false                                    ;
  ///////////////////////////////////////////////////////////////////////////
  int savedOffset = ( *offset )                                             ;
  invalidate ( )                                                            ;
  if  ( ! nextTagIs ( VALUE_TAG, valueXml, offset ) ) return false          ;
  int     afterValueOffset = ( *offset )                                    ;
  QString typeTag          = getNextTag ( valueXml , offset )               ;
  bool    result           = false                                          ;
  ///////////////////////////////////////////////////////////////////////////
  if ( typeTag .  isEmpty ( )  ) result = stringFromXml ( valueXml,offset ) ;
  else
  if ( typeTag == STRING_TAG   ) result = stringFromXml ( valueXml,offset ) ;
  else
  if ( typeTag == BOOLEAN_TAG  ) result = boolFromXml   ( valueXml,offset ) ;
  else
  if ( typeTag == I4_TAG       ) result = intFromXml    ( valueXml,offset ) ;
  else
  if ( typeTag == I8_TAG       ) result = tuidFromXml   ( valueXml,offset ) ;
  else
  if ( typeTag == U8_TAG       ) result = uuidFromXml   ( valueXml,offset ) ;
  else
  if ( typeTag == INT_TAG      ) result = intFromXml    ( valueXml,offset ) ;
  else
  if ( typeTag == DOUBLE_TAG   ) result = doubleFromXml ( valueXml,offset ) ;
  else
  if ( typeTag == DATETIME_TAG ) result = timeFromXml   ( valueXml,offset ) ;
  else
  if ( typeTag == BASE64_TAG   ) result = binaryFromXml ( valueXml,offset ) ;
  else
  if ( typeTag == ARRAY_TAG    ) result = arrayFromXml  ( valueXml,offset ) ;
  else
  if ( typeTag == STRUCT_TAG   ) result = structFromXml ( valueXml,offset ) ;
  else
  if ( typeTag == VALUE_ETAG   )                                            {
    *offset = afterValueOffset                                              ;
    result  = stringFromXml ( valueXml , offset )                           ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if ( result ) findTag ( VALUE_ETAG , valueXml , offset )                  ;
           else *offset = savedOffset                                       ;
  ///////////////////////////////////////////////////////////////////////////
  return result                                                             ;
}

QString Value::toXml(void)
{
  switch ( _type )                             {
    case TypeBoolean  : return boolToXml   ( ) ;
    case TypeInt      : return intToXml    ( ) ;
    case TypeTuid     : return tuidToXml   ( ) ;
    case TypeUuid     : return uuidToXml   ( ) ;
    case TypeDouble   : return doubleToXml ( ) ;
    case TypeString   : return stringToXml ( ) ;
    case TypeDateTime : return timeToXml   ( ) ;
    case TypeBase64   : return binaryToXml ( ) ;
    case TypeArray    : return arrayToXml  ( ) ;
    case TypeStruct   : return structToXml ( ) ;
    default           : break                  ;
  }                                            ;
  return QString                           ( ) ;
}

bool Value::boolFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false       ;
  //////////////////////////////////////////
  int     BL = valueXml . length ( )       ;
  int     OS = *offset                     ;
  QString BS                               ;
  bool    BM                               ;
  //////////////////////////////////////////
  do                                       {
    QChar c = valueXml . at ( OS )         ;
    BM = c . isDigit ( )                   ;
    if ( BM )                              {
      BS . append ( c )                    ;
      OS++                                 ;
      BM = ( OS < BL )                     ;
    }                                      ;
  } while ( BM )                           ;
  //////////////////////////////////////////
  if ( BS . length ( ) <= 0 ) return false ;
  BM = true                                ;
  BL = BS . toInt ( &BM )                  ;
  if ( BM )                                {
    *offset = OS                           ;
  } else return false                      ;
  //////////////////////////////////////////
  _type  = TypeBoolean                     ;
  asBool = ( BL == 1 )                     ;
  return true                              ;
}

QString Value::boolToXml(void)
{
  QString xml                               ;
  xml  = QString::fromUtf8 ( VALUE_TAG    ) ;
  xml += QString::fromUtf8 ( BOOLEAN_TAG  ) ;
  xml += ( asBool ? "1" : "0"             ) ;
  xml += QString::fromUtf8 ( BOOLEAN_ETAG ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG   ) ;
  return xml                                ;
}

bool Value::intFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false       ;
  //////////////////////////////////////////
  int     BL = valueXml . length ( )       ;
  int     OS = *offset                     ;
  QString BS                               ;
  bool    BM                               ;
  //////////////////////////////////////////
  do                                       {
    QChar c = valueXml . at ( OS )         ;
    if ( c == QChar('-') ) BM = true       ;
    else BM = c . isDigit ( )              ;
    if ( BM )                              {
      BS . append ( c )                    ;
      OS++                                 ;
      BM = ( OS < BL )                     ;
    }                                      ;
  } while ( BM )                           ;
  //////////////////////////////////////////
  if ( BS . length ( ) <= 0 ) return false ;
  BM = true                                ;
  BL = BS . toInt ( &BM )                  ;
  if ( BM )                                {
    *offset = OS                           ;
  } else return false                      ;
  //////////////////////////////////////////
  _type = TypeInt                          ;
  asInt =  BL                              ;
  return true                              ;
}

QString Value::intToXml(void)
{
  QString xml                             ;
  xml  = QString::fromUtf8 ( VALUE_TAG  ) ;
  xml += QString::fromUtf8 ( I4_TAG     ) ;
  xml += QString::number   ( asInt      ) ;
  xml += QString::fromUtf8 ( I4_ETAG    ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG ) ;
  return xml                              ;
}

bool Value::tuidFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false       ;
  //////////////////////////////////////////
  qint64  BL = valueXml . length ( )       ;
  int     OS = *offset                     ;
  QString BS                               ;
  bool    BM                               ;
  //////////////////////////////////////////
  do                                       {
    QChar c = valueXml . at ( OS )         ;
    if ( c == QChar('-') ) BM = true       ;
    else BM = c . isDigit ( )              ;
    if ( BM )                              {
      BS . append ( c )                    ;
      OS++                                 ;
      BM = ( OS < BL )                     ;
    }                                      ;
  } while ( BM )                           ;
  //////////////////////////////////////////
  if ( BS . length ( ) <= 0 ) return false ;
  BM = true                                ;
  BL = BS . toLongLong ( &BM )             ;
  if ( BM )                                {
    *offset = OS                           ;
  } else return false                      ;
  //////////////////////////////////////////
  _type  = TypeTuid                        ;
  asTuid =  BL                             ;
  return true                              ;
}

QString Value::tuidToXml(void)
{
  QString xml                             ;
  xml  = QString::fromUtf8 ( VALUE_TAG  ) ;
  xml += QString::fromUtf8 ( I8_TAG     ) ;
  xml += QString::number   ( asTuid     ) ;
  xml += QString::fromUtf8 ( I8_ETAG    ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG ) ;
  return xml                              ;
}

bool Value::uuidFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false       ;
  //////////////////////////////////////////
  quint64 BL = valueXml . length ( )       ;
  int     OS = *offset                     ;
  QString BS                               ;
  bool    BM                               ;
  //////////////////////////////////////////
  do                                       {
    QChar c = valueXml . at ( OS )         ;
    BM = c . isDigit ( )                   ;
    if ( BM )                              {
      BS . append ( c )                    ;
      OS++                                 ;
      BM = ( OS < BL )                     ;
    }                                      ;
  } while ( BM )                           ;
  //////////////////////////////////////////
  if ( BS . length ( ) <= 0 ) return false ;
  BM = true                                ;
  BL = BS . toULongLong ( &BM )            ;
  if ( BM )                                {
    *offset = OS                           ;
  } else return false                      ;
  //////////////////////////////////////////
  _type  = TypeUuid                        ;
  asUuid =  BL                             ;
  return true                              ;
}

QString Value::uuidToXml(void)
{
  QString xml;
  xml  = QString::fromUtf8 ( VALUE_TAG  ) ;
  xml += QString::fromUtf8 ( U8_TAG     ) ;
  xml += QString::number   ( asUuid     ) ;
  xml += QString::fromUtf8 ( U8_ETAG    ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG ) ;
  return xml                              ;
}

bool Value::doubleFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false       ;
  //////////////////////////////////////////
  int     BL = valueXml . length ( )       ;
  int     OS = *offset                     ;
  double  BD = 0                           ;
  QString BS                               ;
  bool    BM                               ;
  //////////////////////////////////////////
  do                                       {
    QChar c = valueXml . at ( OS )         ;
    if ( c == QChar('.') ) BM = true  ; else
    if ( c == QChar('-') ) BM = true       ;
    else BM = c . isDigit ( )              ;
    if ( BM )                              {
      BS . append ( c )                    ;
      OS++                                 ;
      BM = ( OS < BL )                     ;
    }                                      ;
  } while ( BM )                           ;
  //////////////////////////////////////////
  if ( BS . length ( ) <= 0 ) return false ;
  BM = true                                ;
  BD = BS . toDouble ( &BM )               ;
  if ( BM )                                {
    *offset = OS                           ;
  } else return false                      ;
  //////////////////////////////////////////
  _type    = TypeDouble                    ;
  asDouble = BD                            ;
  return true                              ;
}

QString Value::doubleToXml(void)
{
  QString xml                                    ;
  QString buf                                    ;
  buf . sprintf            ( DBLFMT , asDouble ) ;
  xml  = QString::fromUtf8 ( VALUE_TAG         ) ;
  xml += QString::fromUtf8 ( DOUBLE_TAG        ) ;
  xml += buf                                     ;
  xml += QString::fromUtf8 ( DOUBLE_ETAG       ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG        ) ;
  return xml                                     ;
}

bool Value::stringFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false                   ;
  int epos = valueXml . indexOf ( '<' , *offset )      ;
  if ( epos < 0 ) return false                         ;
  _type           = TypeString                         ;
  QString dec = valueXml.mid((*offset),epos-(*offset)) ;
  asString    = Decode(dec)                            ;
  *offset    += dec . length ( )                       ;
  return true                                          ;
}

QString Value::stringToXml(void)
{
  QString xml                              ;
  xml  = QString::fromUtf8 ( VALUE_TAG   ) ;
  xml += QString::fromUtf8 ( STRING_TAG  ) ;
  xml += Encode            ( asString    ) ;
  xml += QString::fromUtf8 ( STRING_ETAG ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG  ) ;
  return xml                               ;
}

bool Value::timeFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false                              ;
  int epos = valueXml . indexOf ( '<' , *offset )                 ;
  if ( epos < 0 ) return false                                    ;
  QString stime = valueXml . mid ( (*offset) , epos - (*offset) ) ;
  bool    ok    = false                                           ;
  qint64  ast   = stime . toLongLong ( & ok )                     ;
  if ( ! ok     ) return false                                    ;
  asTime   = QDateTime::fromMSecsSinceEpoch ( ast )               ;
  _type    = TypeDateTime                                         ;
  *offset += stime . length ( )                                   ;
  return true                                                     ;
}

QString Value::timeToXml(void)
{
  QString xml                                                 ;
  xml  = QString::fromUtf8 ( VALUE_TAG                      ) ;
  xml += QString::fromUtf8 ( DATETIME_TAG                   ) ;
  xml += QString::number   ( asTime . toMSecsSinceEpoch ( ) ) ;
  xml += QString::fromUtf8 ( DATETIME_ETAG                  ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG                     ) ;
  return  xml                                                 ;
}

bool Value::binaryFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset ) return false                      ;
  int epos = valueXml.indexOf('<',*offset)                ;
  if ( epos < 0 ) return false                            ;
  QString    ass = valueXml.mid((*offset),epos-(*offset)) ;
  QByteArray ASB = ass . toUtf8 ( )                       ;
  _type          = TypeBase64                             ;
  asBinary       = QByteArray::fromBase64 ( ASB )         ;
  *offset       += ass . length ( )                       ;
  return true                                             ;
}

QString Value::binaryToXml(void)
{
  QByteArray ASB = asBinary . toBase64 ( ) ;
  QString    xml                           ;
  xml  = QString::fromUtf8 ( VALUE_TAG   ) ;
  xml += QString::fromUtf8 ( BASE64_TAG  ) ;
  xml += QString::fromUtf8 ( ASB         ) ;
  xml += QString::fromUtf8 ( BASE64_ETAG ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG  ) ;
  return     xml                           ;
}

bool Value::arrayFromXml(QString & valueXml,int * offset)
{
  if ( NULL == offset                               ) return false ;
  if ( ! nextTagIs ( DATA_TAG , valueXml , offset ) ) return false ;
  _type   = TypeArray                                              ;
  asArray . clear ( )                                              ;
  bool c = true                                                    ;
  while ( c )                                                      {
    Value v                                                        ;
    c = v . fromXml ( valueXml , offset )                          ;
    if ( c ) asArray . append( v )                                 ;
    if ( nextTagIs ( DATA_ETAG , valueXml , offset ) )             {
      return nextTagIs ( ARRAY_ETAG , valueXml , offset )          ;
    }                                                              ;
  }                                                                ;
  return nextTagIs ( ARRAY_ETAG , valueXml , offset )              ;
}

QString Value::arrayToXml(void)
{
  QString xml                               ;
  xml  = QString::fromUtf8 ( VALUE_TAG    ) ;
  xml += QString::fromUtf8 ( ARRAY_TAG    ) ;
  xml += QString::fromUtf8 ( DATA_TAG     ) ;
  int s = asArray . count  (              ) ;
  for (int i = 0 ; i < s ; ++i )            {
    xml += asArray [ i ] . toXml ( )        ;
  }                                         ;
  xml += QString::fromUtf8 ( DATA_ETAG    ) ;
  xml += QString::fromUtf8 ( ARRAY_ETAG   ) ;
  xml += QString::fromUtf8 ( VALUE_ETAG   ) ;
  return xml                                ;
}

bool Value::structFromXml(QString & valueXml,int * offset)
{
  invalidate ( )                                             ;
  _type    = TypeStruct                                      ;
  asStruct . clear ( )                                       ;
  while ( nextTagIs ( MEMBER_TAG , valueXml , offset ) )     {
    QString name = parseTag ( NAME_TAG , valueXml , offset ) ;
    Value val ( valueXml , offset )                          ;
    if ( ! val . valid ( ) )                                 {
      invalidate ( )                                         ;
      return false                                           ;
    }                                                        ;
    asStruct [ name ] = val                                  ;
    if ( ! nextTagIs ( MEMBER_ETAG , valueXml , offset ) )   {
      invalidate ( )                                         ;
      return false                                           ;
    }                                                        ;
  }                                                          ;
  return nextTagIs ( STRUCT_ETAG , valueXml , offset )       ;
}

QString Value::structToXml(void)
{
  QString xml                                  ;
  xml    = QString::fromUtf8 ( VALUE_TAG     ) ;
  xml   += QString::fromUtf8 ( STRUCT_TAG    ) ;
  QStringList SSO = asStruct . keys ( )        ;
  for (int i = 0 ; i < SSO . count ( ) ; i++ ) {
    QString S = SSO [ i ]                      ;
    xml += QString::fromUtf8 ( MEMBER_TAG    ) ;
    xml += QString::fromUtf8 ( NAME_TAG      ) ;
    xml += Encode            ( S             ) ;
    xml += QString::fromUtf8 ( NAME_ETAG     ) ;
    xml += asStruct [ S ] . toXml ( )          ;
    xml += QString::fromUtf8 ( MEMBER_ETAG   ) ;
  }                                            ;
  xml   += QString::fromUtf8 ( STRUCT_ETAG   ) ;
  xml   += QString::fromUtf8 ( VALUE_ETAG    ) ;
  return  xml                                  ;
}

QString Value::Decode(QString & encoded)
{
  int iAmp = encoded . indexOf ( AMP )                                   ;
  if ( iAmp < 0 ) return encoded                                         ;
  QString decoded = encoded . left   ( iAmp )                            ;
  int     iSize   = encoded . length (      )                            ;
  ////////////////////////////////////////////////////////////////////////
  while ( iAmp != iSize )                                                {
    if ( ( AMP == encoded . at ( iAmp ) ) && ( ( iAmp + 1 ) < iSize ) )  {
      int iEntity = 0                                                    ;
      while ( 0 != xmlEntity [ iEntity ] )                               {
        QString amp = encoded . mid ( iAmp + 1 , xmlEntLen [ iEntity ] ) ;
        QString ent = QString ( xmlEntity [ iEntity ] )                  ;
        if ( amp == ent )                                                {
          decoded . append ( rawEntity [ iEntity ] )                     ;
          iAmp   += xmlEntLen [ iEntity ] + 1                            ;
          break                                                          ;
        }                                                                ;
        ++iEntity                                                        ;
      }                                                                  ;
      if ( 0 == xmlEntity [ iEntity ] )                                  {
        decoded . append ( encoded . at ( iAmp ) )                       ;
        iAmp++                                                           ;
      }                                                                  ;
    } else                                                               {
      decoded . append ( encoded . at ( iAmp ) )                         ;
      iAmp++                                                             ;
    }                                                                    ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  return decoded                                                         ;
}

QString Value::Encode(QString & raw)
{
  if ( raw . length ( ) <= 0 ) return ""                          ;
  QString RES   ( rawEntity )                                     ;
  RES . prepend ( '['       )                                     ;
  RES . append  ( ']'       )                                     ;
  /////////////////////////////////////////////////////////////////
  QRegExp REX ( RES )                                             ;
  int iRep = raw . indexOf ( REX )                                ;
  if ( iRep < 0 ) return raw                                      ;
  /////////////////////////////////////////////////////////////////
  QString encoded = raw . left ( iRep )                           ;
  int     iSize   = raw . size (      )                           ;
  while ( iRep != iSize )                                         {
    int iEntity = 0                                               ;
    while ( 0 != rawEntity [ iEntity ] )                          {
      if ( raw . at ( iRep ) == QChar ( rawEntity [ iEntity ] ) ) {
        encoded . append ( AMP                   )                ;
        encoded . append ( xmlEntity [ iEntity ] )                ;
        break                                                     ;
      }                                                           ;
      ++iEntity                                                   ;
    }                                                             ;
    if ( 0 == rawEntity [ iEntity ] )                             {
      encoded . append ( raw . at ( iRep ) )                      ;
    }                                                             ;
    ++iRep                                                        ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  return encoded                                                  ;
}

QString Value::parseTag(const char * tag,const QString & xml,int * offset)
{
  if ( NULL == offset            ) return ""    ;
  if ( NULL == tag               ) return ""    ;
  if ( (*offset) >= xml.length() ) return ""    ;
  ///////////////////////////////////////////////
  QString Tag    = QString::fromUtf8(tag)       ;
  if ( Tag . length ( ) <= 0   ) return ""      ;
  int istart = xml . indexOf ( Tag , *offset )  ;
  if ( istart < 0 ) return ""                   ;
  istart += Tag.length()                        ;
  QString etag = "</"                           ;
  QString TOX  = QString::fromUtf8(tag+1)       ;
  if ( TOX . length ( ) <= 0   ) return ""      ;
  etag . append ( TOX )                         ;
  int iend = xml . indexOf ( etag , istart )    ;
  if ( iend < 0 ) return ""                     ;
  (*offset) = (int)( iend + etag . length ( ) ) ;
  return xml . mid ( istart , iend - istart )   ;
}

bool Value::findTag(const char * tag,const QString & xml,int * offset)
{
  if ( NULL == offset                ) return false ;
  if ( NULL == tag                   ) return false ;
  if ( (*offset) >= xml . length ( ) ) return false ;
  ///////////////////////////////////////////////////
  QString Tag = QString::fromUtf8 ( tag )           ;
  int istart = xml . indexOf ( Tag , *offset )      ;
  if ( istart < 0 ) return false                    ;
  (*offset) = (int)( istart + Tag . length ( ) )    ;
  ///////////////////////////////////////////////////
  return true                                       ;
}

bool Value::nextTagIs(const char * tag,const QString & xml,int * offset)
{
  if ( NULL == offset                ) return false ;
  if ( NULL == tag                   ) return false ;
  if ( (*offset) >= xml . length ( ) ) return false ;
  ///////////////////////////////////////////////////
  QString Tag = QString::fromUtf8 ( tag )           ;
  int     len = Tag . length ( )                    ;
  if ( len <= 0                      ) return false ;
  int nc   = ( *offset )                            ;
  int xlen = xml . length ( )                       ;
  while ( ( nc < xlen )                            &&
          ( xml . at ( nc ) . isSpace ( )          ||
            xml . at ( nc ) == QChar('\t')         ||
            xml . at ( nc ) == QChar('\r')         ||
            xml . at ( nc ) == QChar('\n') ) ) ++nc ;
  ///////////////////////////////////////////////////
  QString ml = xml . mid ( nc , len )               ;
  if ( len != ml . length ( ) ) return false        ;
  Tag = Tag . toLower ( )                           ;
  ml  = ml  . toLower ( )                           ;
  if ( ml == Tag )                                  {
    (*offset) = ( nc + len )                        ;
    return true                                     ;
  }                                                 ;
  ///////////////////////////////////////////////////
  return false                                      ;
}

QString Value::getNextTag(const QString & xml,int * offset)
{
  if ( NULL      == offset            ) return ""                         ;
  if ( (*offset) >= xml . length ( )  ) ""                                ;
  int pos = *offset                                                       ;
  while ( ( pos < xml . length ( ) )                                     &&
          ( xml . at ( pos ) . isSpace ( )                               ||
            xml . at ( pos ) == QChar('\t')                              ||
            xml . at ( pos ) == QChar('\r')                              ||
            xml . at ( pos ) == QChar('\n')                     ) ) ++pos ;
  if ( xml . at ( pos ) != QChar('<') ) return ""                         ;
  /////////////////////////////////////////////////////////////////////////
  QString s                                                               ;
  while ( pos < xml . length ( ) && ( xml . at ( pos ) != QChar ('>') ) ) {
    s . append ( xml . at ( pos ) )                                       ;
    ++pos                                                                 ;
  }                                                                       ;
  if ( ( pos < xml . length ( ) ) && ( xml . at ( pos ) == QChar('>') ) ) {
    s . append ( xml . at ( pos ) )                                       ;
    ++pos                                                                 ;
  }                                                                       ;
  (*offset) = pos                                                         ;
  /////////////////////////////////////////////////////////////////////////
  return s                                                                ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
