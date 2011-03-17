TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    printer \
    texteditor \
    padtools \
    calendar


core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += padtools
mainwindow.depends += printer

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

texteditor.subdir   = texteditorplugin
texteditor.depends += core

padtools.subdir   = padtoolsplugin
padtools.depends += core

calendar.subdir   = calendarplugin
calendar.depends += core
