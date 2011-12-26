TEMPLATE        = lib
TARGET          = Script

DEFINES += SCRIPT_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( scriptplugin_dependencies.pri )

HEADERS = scriptplugin.h script_exporter.h \
    scriptwrappers.h \
    scriptmanager.h \
    scriptpatientwrapper.h

SOURCES = scriptplugin.cpp \
    scriptwrappers.cpp \
    scriptmanager.cpp \
    scriptpatientwrapper.cpp

OTHER_FILES = Script.pluginspec \
    $${SOURCES_GLOBAL_RESOURCES}/test.js

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/scriptplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/scriptplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/scriptplugin_es.ts






