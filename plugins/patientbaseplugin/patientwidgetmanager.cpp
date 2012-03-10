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
#include "patientwidgetmanager.h"
#include "constants_menus.h"
#include "constants_trans.h"
#include "patientbase.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

using namespace Patients::Constants;
using namespace Patients::Internal;
using namespace Patients;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
PatientWidgetManager *PatientWidgetManager::m_Instance = 0;

PatientWidgetManager *PatientWidgetManager::instance()
{
    if (!m_Instance)
        m_Instance = new PatientWidgetManager(qApp);
    return m_Instance;
}

PatientWidgetManager::PatientWidgetManager(QObject *parent) : PatientActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("PatientWidgetManager");
}

void PatientWidgetManager::updateContext(Core::IContext *object)
{
    PatientSelector *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;
            break;
        }
        view = qobject_cast<PatientSelector *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        PatientActionHandler::setCurrentView(view);
    }
}

void PatientWidgetManager::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    m_CurrentView->init();
}

PatientSelector *PatientWidgetManager::selector() const
{
    return PatientActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actions :
// Toggle search patient view
// Search method ?
// Limit database usage ?
// View patients information
// Patient selection history
PatientActionHandler::PatientActionHandler(QObject *parent) :
        QObject(parent),
        aSearchName(0),
        aSearchFirstname(0),
        aSearchNameFirstname(0),
        aSearchDob(0),
        aViewPatientInformations(0),
        aPrintPatientInformations(0),
        aShowPatientDatabaseInformations(0),
        gSearchMethod(0)
{
    setObjectName("PatientActionHandler");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(Patients::Constants::C_PATIENTS);
    QList<int> searchcontext = QList<int>() << uid->uniqueIdentifier(Patients::Constants::C_PATIENTS_SEARCH) << Core::Constants::C_GLOBAL_ID;
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_PATIENTS);
    if (!menu) {
        Utils::warningMessageBox(tr("Fatal Error"), tr("%1 : Unable to retreive patients menu.").arg(objectName()));
        return;
    }
    Q_ASSERT(menu);
    if (!menu)
        return;

    menu->appendGroup(Constants::G_PATIENTS);
    menu->appendGroup(Constants::G_PATIENTS_NEW);
    menu->appendGroup(Constants::G_PATIENTS_SEARCH);
    menu->appendGroup(Constants::G_PATIENTS_HISTORY);
    menu->appendGroup(Constants::G_PATIENTS_INFORMATIONS);

//    actionManager()->actionContainer(Core::Constants::M_PATIENTS)->addMenu(menu, Core::Constants::G_PATIENTS);

    // Search method menu
    Core::ActionContainer *searchmenu = actionManager()->actionContainer(Constants::M_PATIENTS_SEARCH);
    if (!searchmenu) {
        searchmenu = actionManager()->createMenu(Constants::M_PATIENTS_SEARCH);
        searchmenu->appendGroup(Constants::G_PATIENTS_SEARCH);
        searchmenu->setTranslations(Trans::Constants::SEARCHMENU_TEXT);
        menu->addMenu(searchmenu, Constants::G_PATIENTS_SEARCH);
    }
    Q_ASSERT(searchmenu);

    /** \todo create search icons */
    gSearchMethod = new QActionGroup(this);
    a = aSearchName = new QAction(this);
    a->setObjectName("aSearchName");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Constants::A_SEARCH_PATIENTS_BY_NAME, searchcontext);
    cmd->setTranslations(Constants::SEARCHBYNAME_TEXT, Constants::SEARCHBYNAME_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Constants::G_PATIENTS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchFirstname = new QAction(this);
    a->setObjectName("aSearchFirstname");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Constants::A_SEARCH_PATIENTS_BY_FIRSTNAME, searchcontext);
    cmd->setTranslations(Constants::SEARCHBYFIRSTNAME_TEXT, Constants::SEARCHBYFIRSTNAME_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Constants::G_PATIENTS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchNameFirstname = new QAction(this);
    a->setObjectName("aSearchNameFirstname");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Constants::A_SEARCH_PATIENTS_BY_NAMEFIRSTNAME, searchcontext);
    cmd->setTranslations(Constants::SEARCHBYNAMEFIRSTNAME_TEXT, Constants::SEARCHBYNAMEFIRSTNAME_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Constants::G_PATIENTS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchDob = new QAction(this);
    a->setObjectName("aSearchDob");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Constants::A_SEARCH_PATIENTS_BY_DOB, searchcontext);
    cmd->setTranslations(Constants::SEARCHBYDOB_TEXT, Constants::SEARCHBYDOB_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Constants::G_PATIENTS_SEARCH);
    gSearchMethod->addAction(a);

    connect(gSearchMethod, SIGNAL(triggered(QAction*)), this, SLOT(searchActionChanged(QAction*)));


//    a = aViewPatientInformations = new QAction(this);
//    a->setObjectName("aViewPatientInformations");
////    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
//    cmd = actionManager()->registerAction(a, Constants::A_VIEWPATIENT_INFOS, globalcontext);
//    cmd->setTranslations(Trans::Constants::PATIENT_INFORMATION);
//    menu->addAction(cmd, Constants::G_PATIENTS_INFORMATIONS);
////    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    if (!hmenu)
        return;

    a = aShowPatientDatabaseInformations = new QAction(this);
    a->setObjectName("aShowPatientDatabaseInformations");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_VIEWPATIENTDATABASE_INFOS, globalcontext);
    cmd->setTranslations(Trans::Constants::PATIENT_DATABASE);
    cmd->retranslate();
    hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);

    connect(aShowPatientDatabaseInformations,SIGNAL(triggered()), this, SLOT(showPatientDatabaseInformations()));

//    contextManager()->updateContext();
//    actionManager()->retranslateMenusAndActions();
}

void PatientActionHandler::updateActions()
{}

void PatientActionHandler::setCurrentView(PatientSelector *view)
{
    m_CurrentView = view;
}

void PatientActionHandler::searchActionChanged(QAction *action)
{
    if (action==aSearchName) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByName);
    }
    if (action==aSearchFirstname) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByFirstname);
    }
    if (action==aSearchNameFirstname) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByNameFirstname);
    }
    if (action==aSearchDob) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByDOB);
    }
}

void PatientActionHandler::viewPatientInformations()
{}

void PatientActionHandler::printPatientsInformations()
{}

void PatientActionHandler::showPatientDatabaseInformations()
{
    QDialog dlg(qApp->activeWindow(), Qt::WindowFlags(Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint));
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    PatientBase::instance()->toTreeWidget(&tree);
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}
