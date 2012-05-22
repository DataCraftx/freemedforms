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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "formmanagermode.h"
#include "formplaceholder.h"
#include "iformitem.h"
#include "iformio.h"
#include "formmanager.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <extensionsystem/pluginmanager.h>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

using namespace Form;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager *formManager() {return Form::FormManager::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ModeManager *modeManager()  { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager()  { return Core::ICore::instance()->actionManager(); }
static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }

/**
  \class Form::Internal::FormManagerMode
  Mode for the central form files (called "Patient files").
*/
FormManagerMode::FormManagerMode(QObject *parent) :
    Core::BaseMode(parent),
    m_inPluginManager(false),
    m_actionInBar(false)
{
    setName(tr("Patients Files"));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTFILES, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_FILE);
    setUniqueModeName(Core::Constants::MODE_PATIENT_FILE);
    setPatientBarVisibility(true);

    m_Holder = new FormPlaceHolder;
    m_Holder->setObjectName("EpisodesFormPlaceHolder");
//    m_Holder->installEventFilter(this);

//    const QList<int> &context;
//    setContext();
    setWidget(m_Holder);
    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(getPatientForm()));
}

FormManagerMode::~FormManagerMode()
{
    if (m_inPluginManager) {
        pluginManager()->removeObject(this);
    }
    // m_Holder is deleted by Core::BaseMode
}

QString FormManagerMode::name() const
{
    return tr("Patients Files");
}

/**
  \brief Get the patient form from the episode database, send the load signal with the form absPath and load it.
  \sa Core::ICore::loadPatientForms()
*/
bool FormManagerMode::getPatientForm()
{
    if (!m_inPluginManager) {
        pluginManager()->addObject(this);
        m_inPluginManager = true;
    }
    // TODO: code here : add patient synthesis action in fancy action bar
//    if (!m_actionInBar) {
//        Core::Command *cmd = actionManager()->command(Constants::A_SHOWPATIENTSYNTHESIS);
//        modeManager()->addAction(cmd, 100);
//        m_actionInBar = true;
//    }
    Form::FormMain *root = formManager()->rootForm(Core::Constants::MODE_PATIENT_FILE);
    m_Holder->setRootForm(root);
    return (root);
}

//bool FormManagerMode::eventFilter(QObject *obj, QEvent *event)
//{
//    qWarning() << obj << event->type();
//    if (obj==m_Holder) {
//        qWarning() << "GET HOLDER";
//        if (event->type()==QEvent::Show) {
//            qWarning() << "GET SHOW";
//            mainWindow()->endProcessingSpinner();
//        }
//    }
//    return QObject::eventFilter(obj, event);
//}
