DEFINES += DRUGINTERACTIONS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include(druginteractionsplugin_dependencies.pri)

HEADERS = $${PWD}/druginteractionsplugin.h $${PWD}/druginteractions_exporter.h \
    $${PWD}/druginteractionsbase.h \
    $${PWD}/allergyengine.h \
    $${PWD}/drugdruginteractionengine.h \
    $${PWD}/pimengine.h

SOURCES = $${PWD}/druginteractionsplugin.cpp \
    $${PWD}/druginteractionsbase.cpp \
    $${PWD}/allergyengine.cpp \
    $${PWD}/drugdruginteractionengine.cpp \
    $${PWD}/pimengine.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/druginteractionsplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/druginteractionsplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/druginteractionsplugin_es.ts
