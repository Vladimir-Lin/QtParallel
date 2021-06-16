# UNIX interfaces

message("UNIX system interface included")

INCLUDEPATH += $${PWD}

SOURCES     += $${PWD}/unix-talk.cpp
SOURCES     += $${PWD}/unix-neighbors.cpp
SOURCES     += $${PWD}/unix-listener.cpp
SOURCES     += $${PWD}/unix-server.cpp
SOURCES     += $${PWD}/unix-client.cpp
