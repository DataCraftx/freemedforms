# toolkit library project file
TEMPLATE         = lib
TARGET           = MedinTux

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

# include config file
include( ../libsworkbench.pri )
include( ../medintuxutils_dependencies.pri )

# define Qt
QT *= sql
CONFIG *= dll

# make library exportable
DEFINES *= MEDINTUX_LIBRARY

# Core
HEADERS += configuration.h \
    medintux_exporter.h

SOURCES += configuration.cpp

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/medintuxtoolkit_fr.ts \
                $${SOURCES_TRANSLATIONS}/medintuxtoolkit_de.ts \
                $${SOURCES_TRANSLATIONS}/medintuxtoolkit_es.ts
