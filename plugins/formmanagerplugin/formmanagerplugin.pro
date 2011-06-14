TEMPLATE = lib
TARGET = FormManager

DEFINES += FORMMANAGER_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( formmanagerplugin_dependencies.pri )
HEADERS = formmanagerplugin.h \
    formmanager_exporter.h \
    formmanager.h \
    iformio.h \
    iformitem.h \
    iformitemspec.h \
    iformitemscripts.h \
    iformitemvalues.h \
    iformwidgetfactory.h \
    iformitemdata.h \
    formmanagermode.h \
    formplaceholder.h \
    episodemodel.h \
    episodebase.h \
    constants_db.h \
    formfilesselectorwidget.h \
    formmanagerpreferencespage.h \
    formcontextualwidget.h \
    formfile.h \
    formeditordialog.h \
    firstrunformmanager.h \
    formiodescription.h \
    formioquery.h \
    subforminsertionpoint.h
SOURCES = formmanagerplugin.cpp \
    formmanager.cpp \
    iformitem.cpp \
    iformwidgetfactory.cpp \
    formmanagermode.cpp \
    formplaceholder.cpp \
    episodemodel.cpp \
    episodebase.cpp \
    formfilesselectorwidget.cpp \
    formmanagerpreferencespage.cpp \
    iformio.cpp \
    formcontextualwidget.cpp \
    formfile.cpp \
    formeditordialog.cpp \
    firstrunformmanager.cpp

OTHER_FILES = FormManager.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/formmanagerplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/formmanagerplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/formmanagerplugin_es.ts

FORMS += formfilesselectorwidget.ui \
    formeditordialog.ui \
    formmanagerpreferenceswidget.ui
