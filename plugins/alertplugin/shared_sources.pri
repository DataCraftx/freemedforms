TEMPLATE = lib
TARGET = Alert

DEFINES += ALERT_LIBRARY

QT += sql

include(../fmf_plugins.pri)
include(alertplugin_dependencies.pri)

HEADERS += \
    $${PWD}/constants.h \
    $${PWD}/alertplugin.h \
    $${PWD}/alertbase.h \
    $${PWD}/alertcore.h \
    $${PWD}/alertitem.h \
    $${PWD}/ialertplaceholder.h \
    $${PWD}/alertitemeditorwidget.h \
    $${PWD}/alertitemtimingeditorwidget.h \
    $${PWD}/alertitemeditordialog.h \
    $${PWD}/blockingalertdialog.h \
    $${PWD}/alertplaceholdertest.h \
    $${PWD}/nonblockingalertwidgets.h \
    $${PWD}/alertscriptmanager.h \
    $${PWD}/alertitemscripteditor.h

SOURCES += \
    $${PWD}/alertplugin.cpp \
    $${PWD}/alertbase.cpp \
    $${PWD}/alertcore.cpp \
    $${PWD}/alertitem.cpp \
    $${PWD}/ialertplaceholder.cpp \
    $${PWD}/alertitemeditorwidget.cpp \
    $${PWD}/alertitemtimingeditorwidget.cpp \
    $${PWD}/alertitemeditordialog.cpp \
    $${PWD}/blockingalertdialog.cpp \
    $${PWD}/alertplaceholdertest.cpp \
    $${PWD}/nonblockingalertwidgets.cpp \
    $${PWD}/alertscriptmanager.cpp \
    $${PWD}/alertitemscripteditor.cpp

FORMS += \
    $${PWD}/alertitemeditorwidget.ui \
    $${PWD}/alertitemtimingeditorwidget.ui \
    $${PWD}/alertitemeditordialog.ui \
    $${PWD}/blockingalertdialog.ui \
    $${PWD}/blockingalertdialogoverridingcomment.ui \
    $${PWD}/alertitemscripteditor.ui

OTHER_FILES += $${PWD}/Alert.pluginspec

TRANSLATIONS += \
    $${SOURCES_TRANSLATIONS_PATH}/alertplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/alertplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/alertplugin_es.ts
