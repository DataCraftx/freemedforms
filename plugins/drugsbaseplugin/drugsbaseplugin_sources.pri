DEFINES += DRUGSBASE_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/drugsbaseplugin_dependencies.pri )

HEADERS += $${PWD}/drugsbaseplugin.h \
    $${PWD}/drugsbase_exporter.h \
    $${PWD}/drugsbase.h \
    $${PWD}/interactionmanager.h \
    $${PWD}/constants.h \
    $${PWD}/drugsmodel.h \
    $${PWD}/dosagemodel.h \
    $${PWD}/drugsio.h \
    $${PWD}/globaldrugsmodel.h \
    $${PWD}/dailyschememodel.h \
    $${PWD}/versionupdater.h \
    $${PWD}/drugstemplateprinter.h \
    $${PWD}/drugsdatabaseselector.h \
    $${PWD}/atctreemodel.h \
    $${PWD}/drugsearchengine.h \
    $${PWD}/constants_databaseschema.h \
    $${PWD}/idrugengine.h \
    $${PWD}/idruginteraction.h \
    $${PWD}/idrug.h \
    $${PWD}/drugdruginteractionengine.h \
    $${PWD}/drugbasecore.h \
    $${PWD}/protocolsbase.h \
    $${PWD}/druginteractionquery.h \
    $${PWD}/idrugprescription.h \
    $${PWD}/ivirtualdrug.h \
    $${PWD}/druginteractionresult.h \
    $${PWD}/idruginteractionalert.h \
    $${PWD}/druginteractioninformationquery.h \
    $${PWD}/iprescription.h

#    $${PWD}/interactionsbase.h \
#    $${PWD}/drugsdatabaseaboutpage.h \
#    $${PWD}/drugsinteraction.h \

SOURCES += $${PWD}/drugsbaseplugin.cpp \
    $${PWD}/drugsbase.cpp \
    $${PWD}/interactionmanager.cpp \
    $${PWD}/drugsmodel.cpp \
    $${PWD}/dosagemodel.cpp \
    $${PWD}/drugsio.cpp \
    $${PWD}/globaldrugsmodel.cpp \
    $${PWD}/dailyschememodel.cpp \
    $${PWD}/versionupdater.cpp \
    $${PWD}/drugstemplateprinter.cpp \
    $${PWD}/drugsdatabaseselector.cpp \
    $${PWD}/atctreemodel.cpp \
    $${PWD}/drugsearchengine.cpp \
    $${PWD}/idrug.cpp \
    $${PWD}/drugdruginteractionengine.cpp \
    $${PWD}/drugbasecore.cpp \
    $${PWD}/protocolsbase.cpp \
    $${PWD}/idruginteractionalert.cpp \
    $${PWD}/druginteractionquery.cpp \
    $${PWD}/druginteractionresult.cpp \
    $${PWD}/druginteractioninformationquery.cpp

#    $${PWD}/interactionsbase.cpp \
#    $${PWD}/drugsdatabaseaboutpage.cpp \
#    $${PWD}/drugsinteraction.cpp \


# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/drugsbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/drugsbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/drugsbaseplugin_es.ts
