TEMPLATE  = lib
TARGET    = quazip

DEFINES += QUAZIP_LIBRARY

include( ../../../libs/libsworkbench.pri )

#isEmpty(BUILDING_PROTECTED):include( ../../../config.pri )
#include( ../../../config.pri )

#DESTDIR = ../

CONFIG *= dll
#CONFIG *= staticlib
# CONFIG *= qt warn_on staticlib
# QT -= gui

# include config file

#CONFIG *= dll

unix {
    LIBS *= -lz
} else {
    message( Win32 compilation of Quazip )
    PRE_TARGETDEPS += ../zlib-1.2.3
    LIBS *= -L../zlib-1.2.3 -lz
}

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += crypt.h \
           ioapi.h \
           quazip.h \
           quazipfile.h \
           quazipfileinfo.h \
           quazipnewinfo.h \
           unzip.h \
           zip.h \
           exporter.h

SOURCES += ioapi.c \
           quazip.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c
