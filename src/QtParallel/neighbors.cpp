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

Neighbors:: Neighbors (void)
          : udpPort   (0   )
{
}

Neighbors::~Neighbors (void)
{
}

void Neighbors::NeighborError(int level,int code)
{
}

bool Neighbors::DetectMyself(void)
{
  return true ;
}

bool Neighbors::ProbeNeighbors(int state)
{
  return ProbeUdp     ( state ) ;
}

bool Neighbors::WaitNeighbors(void)
{
  return BindUdp      (       ) ;
}

bool Neighbors::FindNeighbors(void)
{
  return BroadcastUdp (       ) ;
}

int Neighbors::NeighborBufferSize (void)
{
  return 1024 ;
}

int Neighbors::ReceiveBufferSize(void)
{
  return 65536 ;
}

int Neighbors::SendingBufferSize(void)
{
  return 16384 ;
}

int Neighbors::NoteTo(int m,QByteArray & d)
{ Q_UNUSED ( m ) ;
  Q_UNUSED ( d ) ;
  return 0       ;
}

int Neighbors::NoteFrom(int m,QByteArray & d)
{ Q_UNUSED ( m ) ;
  Q_UNUSED ( d ) ;
  return 0       ;
}

bool Neighbors::AbortNeighbors(int m)
{ Q_UNUSED ( m ) ;
  return false   ;
}

bool Neighbors::NeighborClosed(int m)
{ Q_UNUSED ( m ) ;
  return false   ;
}

bool Neighbors::NeighborGaps(int m)
{ Q_UNUSED ( m  ) ;
  mpsleep  ( 10 ) ;
  return true     ;
}

bool Neighbors::ProbeUdp(int state)
{
  int                port  = udpPort                                         ;
  int                len   = sizeof(sockaddr_in)                             ;
  int                s     = BroadcastSocket ( )                             ;
  bool               go    = true                                            ;
  int                bsize = NeighborBufferSize ( )                          ;
  QByteArray         R ( bsize + 1 , 0 )                                     ;
  char             * buff  = R . data ( )                                    ;
  int                rsize = 0                                               ;
  int                size                                                    ;
  QByteArray         E                                                       ;
  struct sockaddr_in a                                                       ;
  struct sockaddr_in b                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  if ( s < 0 )                                                               {
    CloseUdp          ( s            )                                       ;
    NeighborClosed    ( WaitNeighbor )                                       ;
    return false                                                             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AnyAddress          ( a , port                                           ) ;
  BroadcastAddress    ( b , port                                           ) ;
  ::bind              ( s , (sockaddr *) &a , sizeof(sockaddr_in)          ) ;
  setNonblock         ( s                                                  ) ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && ( ! AbortNeighbors ( ProbeNeighbor ) ) )                     {
    switch ( state )                                                         {
      case 101                                                               :
        if ( uselect ( s , 10000 ) )                                         {
          ::memset ( buff , 0 , bsize + 1 )                                  ;
          ::recvfrom                                                         (
            s                                                                ,
            buff                                                             ,
            bsize                                                            ,
            0                                                                ,
            (struct sockaddr *) & a                                          ,
            &len                                                           ) ;
          rsize = ::strlen ( buff )                                          ;
          if ( rsize > 0 )                                                   {
            QByteArray B ( buff , rsize )                                    ;
            NoteFrom ( ProbeNeighbor , B )                                   ;
          } else                                                             {
            QByteArray Z                                                     ;
            NoteFrom ( ProbeNeighbor , Z )                                   ;
          }                                                                  ;
        }                                                                    ;
        state = 102                                                          ;
      break                                                                  ;
      case 102                                                               :
        E    . clear  (                   )                                  ;
        size = NoteTo ( ProbeNeighbor , E )                                  ;
        if ( size > 0 )                                                      {
          ::sendto ( s                                                       ,
                     E . constData ( )                                       ,
                     size                                                    ,
                     0                                                       ,
                     (sockaddr *) & b                                        ,
                     len                                                   ) ;
        }                                                                    ;
        state = 101                                                          ;
      break                                                                  ;
      default                                                                :
        state = 101                                                          ;
      break                                                                  ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    go = NeighborGaps  ( ProbeNeighbor    )                                  ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CloseUdp             ( s                )                                  ;
  NeighborClosed       ( ProbeNeighbor    )                                  ;
  ////////////////////////////////////////////////////////////////////////////
  return DetectMyself  (                  )                                  ;
}

bool Neighbors::BindUdp(void)
{
  struct sockaddr_in a                                                       ;
  struct sockaddr_in b                                                       ;
  int                s     = BroadcastSocket ( )                             ;
  int                port  = udpPort                                         ;
  int                len   = sizeof(sockaddr_in)                             ;
  bool               go    = true                                            ;
  int                bsize = NeighborBufferSize ( )                          ;
  int                rsize = 0                                               ;
  QByteArray         R ( bsize + 1 , 0 )                                     ;
  char             * buff  = R . data ( )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( s < 0 )                                                               {
    CloseUdp          ( s            )                                       ;
    NeighborClosed    ( WaitNeighbor )                                       ;
    return false                                                             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AnyAddress          ( a , port                                           ) ;
  BroadcastAddress    ( b , port                                           ) ;
  ::bind              ( s , (sockaddr *) &a , sizeof(sockaddr_in)          ) ;
  setNonblock         ( s                                                  ) ;
  ////////////////////////////////////////////////////////////////////////////
  while ( go && ( ! AbortNeighbors ( WaitNeighbor ) ) )                      {
    if ( uselect ( s , 10000 ) )                                             {
      ::memset ( buff , 0 , bsize + 1 )                                      ;
      ::recvfrom                                                             (
        s                                                                    ,
        buff                                                                 ,
        bsize                                                                ,
        0                                                                    ,
        (struct sockaddr *) & b                                              ,
        &len                                                               ) ;
      rsize = ::strlen ( buff             )                                  ;
      if               ( rsize > 0        )                                  {
        QByteArray B   ( buff , rsize     )                                  ;
        NoteFrom       ( WaitNeighbor , B )                                  ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    go = NeighborGaps  ( WaitNeighbor     )                                  ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CloseUdp             ( s                )                                  ;
  NeighborClosed       ( WaitNeighbor     )                                  ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool Neighbors::BroadcastUdp(void)
{
  int                port  = udpPort                                         ;
  int                len   = sizeof(sockaddr_in)                             ;
  bool               go    = true                                            ;
  int                s     = BroadcastSocket ( )                             ;
  int                size                                                    ;
  QByteArray         E                                                       ;
  struct sockaddr_in a                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  if ( s < 0 )                                                               {
    CloseUdp          ( s            )                                       ;
    NeighborClosed    ( FindNeighbor )                                       ;
    return false                                                             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  BroadcastAddress         ( a , port                                      ) ;
  setNonblock              ( s                                             ) ;
  while ( go && ( ! AbortNeighbors ( FindNeighbor ) ) )                      {
    E    . clear  (                  )                                       ;
    size = NoteTo ( FindNeighbor , E )                                       ;
    if ( size > 0 )                                                          {
      ::sendto ( s                                                           ,
                 E . constData ( )                                           ,
                 size                                                        ,
                 0                                                           ,
                 (sockaddr *) & a                                            ,
                 len                                                       ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    go = NeighborGaps ( FindNeighbor )                                       ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CloseUdp            ( s            )                                       ;
  NeighborClosed      ( FindNeighbor )                                       ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

//////////////////////////////////////////////////////////////////////////////

END_PARALLEL_NAMESPACE
QT_END_NAMESPACE
