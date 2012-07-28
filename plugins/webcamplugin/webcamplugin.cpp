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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "webcamplugin.h"
#include "webcamconstants.h"
#include "webcamphotoprovider.h"
#include "webcampreferences.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Webcam;
using namespace Internal;

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

WebcamPlugin::WebcamPlugin() :
    ExtensionSystem::IPlugin(),
    m_prefPage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating Webcam";
    
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("webcam"); 
    
    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
    
    // All preferences pages must be created in this part (before user connection)
    m_prefPage = new WebcamPreferencesPage(this);
    // And included in the QObject pool
    addObject(m_prefPage);
    
    m_webcamProvider = new WebcamPhotoProvider();

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

WebcamPlugin::~WebcamPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members

    if(m_webcamProvider) {
        removeObject(m_webcamProvider);
        delete m_webcamProvider;
    }
    if (m_prefPage) {
        removeObject(m_prefPage);
        delete(m_prefPage);
    }
}

bool WebcamPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    if (Utils::Log::warnPluginsCreation()) {
        qDebug() << "WebcamPlugin::initialize";
    }
    
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // connect to other plugins' signals
    // "In the initialize method, a plugin can be sure that the plugins it
    //  depends on have initialized their members."
    
    // FreeMedForms:
    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool
    
    // No user is logged in until here
    

    ExtensionSystem::PluginManager::instance()->addObject(m_webcamProvider);

    //    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    //
    //    QAction *action = new QAction(tr("Webcam action"), this);
    //    Core::Command *cmd = am->registerAction(action, Constants::ACTION_ID,
    //                         Core::Context(Core::Constants::C_GLOBAL));
    //    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Shift+Meta+A")));
    //    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));
    //
    //    Core::ActionContainer *menu = am->createMenu(Constants::CUSTOM_MENU_ID);
    //    menu->menu()->setTitle(tr("Webcam"));
    //    menu->addAction(cmd);
    //    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
    
    return true;
}

void WebcamPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation()) {
        qDebug() << "Webcam::extensionsInitialized";
    }
    
    // Add Translator to the Application
    //Core::ICore::instance()->translators()->addNewTranslator("mf_XXX_Widget");
    
    // Retrieve other objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
    
    // If you want to stop the plugin initialization if there are no identified user
    // Just uncomment the following code
    //    // no user -> stop here
    //    if (!user())
    //        return;
    //    if (user()->uuid().isEmpty())
    //        return;
    
    messageSplash(tr("Initializing Webcam..."));
    
    // At this point, user is connected
    
    // Add here e.g. the DataPackPlugin::IDataPackListener objects to the pluginmanager object pool
    
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

void WebcamPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

// aboutToShutdown does not exist in the old QtCreator code.
// we have to wait until FMF is updatet to a newer QtCreator source
// ExtensionSystem::IPlugin::ShutdownFlag WebcamPlugin::aboutToShutdown()
// {
//     // Save settings
//     // Disconnect from signals that are not needed during shutdown
//     // Hide UI (if you add UI that is not in the main window directly)
//     return SynchronousShutdown;
// }

// void WebcamPlugin::triggerAction()
// {
//     QMessageBox::information(Core::ICore::instance()->mainWindow(),
//                              tr("Action triggered"),
//                              tr("This is an action from Webcam."));
// }

void WebcamPlugin::coreAboutToClose()
{
    // Core is about to close
    // ICore::user() is still available
}

Q_EXPORT_PLUGIN2(Webcam, WebcamPlugin)

