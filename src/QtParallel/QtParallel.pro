NAME         = QtParallel
TARGET       = $${NAME}
QT           = core
QT          -= gui
QT          -= script
QT          += network
QT          += QtUUID
QT          += QtZMQ

load(qt_build_config)
load(qt_module)

# DEFINES     += NO_PARALLEL_NAMESPACE
# DEFINES     += NO_SYSTEM_METHODS

INCLUDEPATH += $${PWD}/../../include/QtParallel

HEADERS     += $${PWD}/../../include/QtParallel/QtParallel
HEADERS     += $${PWD}/../../include/QtParallel/qtparallel.h
HEADERS     += $${PWD}/parallel.hpp

SOURCES     += $${PWD}/qtparallel.cpp
SOURCES     += $${PWD}/value.cpp
SOURCES     += $${PWD}/extras.cpp
SOURCES     += $${PWD}/header.cpp
SOURCES     += $${PWD}/companion.cpp
SOURCES     += $${PWD}/method.cpp
SOURCES     += $${PWD}/listing.cpp
SOURCES     += $${PWD}/help.cpp
SOURCES     += $${PWD}/exists.cpp
SOURCES     += $${PWD}/chatmethod.cpp
SOURCES     += $${PWD}/filemethod.cpp
SOURCES     += $${PWD}/methods.cpp
SOURCES     += $${PWD}/companionmethod.cpp
SOURCES     += $${PWD}/talk.cpp
SOURCES     += $${PWD}/client.cpp
SOURCES     += $${PWD}/server.cpp
SOURCES     += $${PWD}/listener.cpp
SOURCES     += $${PWD}/neighbors.cpp
SOURCES     += $${PWD}/dispatcher.cpp
SOURCES     += $${PWD}/nIpAddress.cpp

win32 {

include ($${PWD}/Windows/Windows.pri)

LIBS        += -lws2_32

}

unix {

include ($${PWD}/Unix/Unix.pri)

}

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri

include ($${PWD}/../../doc/Qt/Qt.pri)
