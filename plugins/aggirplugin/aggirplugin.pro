TEMPLATE = lib
TARGET = Gir

DEFINES += AGGIR_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( aggirplugin_dependencies.pri )
HEADERS = \
    aggirplugin.h \
    aggir_exporter.h \
    girwidget.h \
    girmodel.h

SOURCES = \
    aggirplugin.cpp \
    girwidget.cpp \
    girmodel.cpp

OTHER_FILES = Gir.pluginspec

FORMS += girwidget.ui
