/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <QtPlugin>
#include <QApplication>
#include <QTextCodec>
#include <QDir>

#include <QLibrary>

#include <QDebug>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>

#include <utils/log.h>
#include <utils/database.h>

#include <iostream>

enum { WarnAllPluginSpecs = false};

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;


static const char * COREPLUGINSNAME = "Core";

static const QString HELP_MESSAGE =
        QString("");

static const QString VERSION_MESSAGE =
        QString("FreeDiams %1 - %2 ; build on %3 %4 \n  %5 \n  Compiled with Qt: %6 - Running with Qt: %7")
        .arg(PACKAGE_VERSION)
#ifdef LINUX_INTEGRATED
#  ifdef DEBUG
        .arg("Debug (Linux Integrated)")
#  else
        .arg("Release (Linux Integrated)")
#  endif
#else  // NOT LINUX_INTEGRATED
#  ifdef DEBUG
        .arg("Debug")
#  else
        .arg("Release")
#  endif
#endif
        .arg(__DATE__, __TIME__)
#ifdef FULLAPPLICATION_BUILD
        .arg("Full application")
#else
        .arg("SVN application")
#endif
        .arg(QT_VERSION_STR)
        .arg(qVersion());


static inline QString getPluginPaths()
{
    QString app = qApp->applicationDirPath();

#ifdef DEBUG_WITHOUT_INSTALL
#    ifdef Q_OS_MAC
        app = QDir::cleanPath(app+"/../../../");
#    endif
    app += "/plugins/";
    return app;
#endif

#ifdef LINUX_INTEGRATED
    app = QString(BINARY_NAME).remove("_debug").toLower();
    return QString("/usr/%1/%2").arg(LIBRARY_BASENAME).arg(app);
#endif

#  ifdef Q_OS_MAC
    app = QDir::cleanPath(app+"/../plugins/");
    return app;
#  endif

// TODO: Add FreeBSD pluginPath

#  ifdef Q_OS_WIN
    app = QDir::cleanPath(app + "/plugins/");
    return app;
#  endif

    return QDir::cleanPath(app + "/plugins/");
}

static inline void defineLibraryPaths()
{
#ifdef LINUX_INTEGRATED
    qApp->addLibraryPath(getPluginPaths());
#else
#  ifndef DEBUG_WITHOUT_INSTALL
    qApp->setLibraryPaths(QStringList() << getPluginPaths() << QDir::cleanPath(getPluginPaths() + "/qt"));
#  endif
#endif
}

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);
    qWarning();

    // --exchange-in=../../../../global_resources/textfiles/dataintest.xml --config=../../../../global_resources/config.ini

    // Add some debugging information
    defineLibraryPaths();
    LOG_FOR("Main","looking for libraries in path : " + qApp->libraryPaths().join(";"));
    Utils::Database::logAvailableDrivers();

    QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );

    app.setApplicationName( QString("%1").arg(BINARY_NAME));
    app.setOrganizationName(BINARY_NAME);
    app.setApplicationVersion(PACKAGE_VERSION);

    QStringList args = qApp->arguments();
    if (args.contains("--version") ||
        args.contains("-version") ||
        args.contains("-v")) {
        std::cout << qPrintable(VERSION_MESSAGE);
        return 0;
    }

    ExtensionSystem::PluginManager pluginManager;
    pluginManager.setFileExtension(QString("pluginspec"));

    QString pluginPaths = getPluginPaths();
    pluginManager.setPluginPaths(QStringList() << pluginPaths);

#ifdef DEBUG
    LOG_FOR("Main", "Running debug version");
#else
    LOG_FOR("Main", "Running release version");
#endif
#ifdef LINUX_INTEGRATED
    LOG_FOR("Main", "Linux Integrated");
#endif

//    const QStringList arguments = app.arguments();
//    QMap<QString, QString> foundAppOptions;
//    if (arguments.size() > 1) {
//        QMap<QString, bool> appOptions;
//        appOptions.insert(QLatin1String(HELP_OPTION1), false);
//        appOptions.insert(QLatin1String(HELP_OPTION2), false);
//        appOptions.insert(QLatin1String(HELP_OPTION3), false);
//        appOptions.insert(QLatin1String(HELP_OPTION4), false);
//        appOptions.insert(QLatin1String(VERSION_OPTION), false);
//        appOptions.insert(QLatin1String(CLIENT_OPTION), false);
//        QString errorMessage;
//        if (!pluginManager.parseOptions(arguments,
//                                        appOptions,
//                                        &foundAppOptions,
//                                        &errorMessage)) {
//            displayError(errorMessage);
//            printHelp(QFileInfo(app.applicationFilePath()).baseName(), pluginManager);
//            return -1;
//        }
//    }

    const PluginSpecSet plugins = pluginManager.plugins();
    ExtensionSystem::PluginSpec *coreplugin = 0;

    if (WarnAllPluginSpecs) {
        foreach (ExtensionSystem::PluginSpec *spec, plugins) {
            qWarning() << "PluginSpecs :::"<< spec->filePath() << spec->name() << spec->version();
        }
    }

    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
        if (spec->name() == QString(COREPLUGINSNAME)) {
            coreplugin = spec;
            break;
        }
    }
    if (!coreplugin) {
        const QString reason = QCoreApplication::translate("Application", "Couldn't find 'Core.pluginspec' in %1").arg(pluginPaths);
        qWarning() << reason;
//        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (coreplugin->hasError()) {
        qWarning() << coreplugin->errorString();
//        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }
//    if (foundAppOptions.contains(QLatin1String(VERSION_OPTION))) {
//        printVersion(coreplugin, pluginManager);
//        return 0;
//    }
//    if (foundAppOptions.contains(QLatin1String(HELP_OPTION1))
//            || foundAppOptions.contains(QLatin1String(HELP_OPTION2))
//            || foundAppOptions.contains(QLatin1String(HELP_OPTION3))
//            || foundAppOptions.contains(QLatin1String(HELP_OPTION4))) {
//        printHelp(QFileInfo(app.applicationFilePath()).baseName(), pluginManager);
//        return 0;
//    }

//    const bool isFirstInstance = !app.isRunning();
//    if (!isFirstInstance && foundAppOptions.contains(QLatin1String(CLIENT_OPTION)))
//        return sendArguments(app, pluginManager.arguments()) ? 0 : -1;

//    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
//        qWarning() << "PlugInSpec" << spec->name() << spec->errorString() << spec->state();
//    }

    pluginManager.loadPlugins();
    if (WarnAllPluginSpecs) {
        foreach (ExtensionSystem::PluginSpec *spec, plugins) {
            qWarning() << "PluginSpecs :::"<< spec->name() << "hasError:" << spec->hasError() << spec->errorString();
        }
    }
    if (coreplugin->hasError()) {
        qWarning() << "main" << coreplugin->errorString();
        return 1;
    }


//    if (isFirstInstance) {
//        // Set up lock and remote arguments for the first instance only.
//        // Silently fallback to unconnected instances for any subsequent
//        // instances.
//        app.initialize();
//        QObject::connect(&app, SIGNAL(messageReceived(QString)), coreplugin->plugin(), SLOT(remoteArgument(QString)));
//    }
//    QObject::connect(&app, SIGNAL(fileOpenRequest(QString)), coreplugin->plugin(), SLOT(remoteArgument(QString)));

    // Do this after the event loop has started
//    QTimer::singleShot(100, &pluginManager, SLOT(startTests()));
    int r = app.exec();
//    Utils::Log::saveLog();
    return r;
}

