DEFINES += EDITOR_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/texteditorplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/texteditor_exporter.h \
    $${PWD}/editoractionhandler.h \
    $${PWD}/editorcontext.h \
    $${PWD}/editormanager.h \
    $${PWD}/tabledialog.h \
    $${PWD}/tableeditor.h \
    $${PWD}/texteditor.h \
    $${PWD}/tablepropertieswidget.h

SOURCES += \
    $${PWD}/editoractionhandler.cpp \
    $${PWD}/editormanager.cpp \
    $${PWD}/tableeditor.cpp \
    $${PWD}/texteditor.cpp \
    $${PWD}/tablepropertieswidget.cpp

FORMS += $${PWD}/tabledialog.ui \
    $${PWD}/tablepropertieswidget.ui \
    $${PWD}/tablepropertiesdialog.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/texteditorplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/texteditorplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/texteditorplugin_es.ts
