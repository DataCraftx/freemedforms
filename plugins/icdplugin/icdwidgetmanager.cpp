/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icdwidgetmanager.h"
#include "constants.h"
#include "icddownloader.h"
#include "icdcentralwidget.h"

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

#include <QGridLayout>
#include <QDialog>
#include <QTreeWidget>
#include <QHeaderView>

using namespace ICD;
using namespace Internal;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
IcdWidgetManager *IcdWidgetManager::m_Instance = 0;

IcdWidgetManager *IcdWidgetManager::instance()
{
    if (!m_Instance)
        m_Instance = new IcdWidgetManager(qApp);
    return m_Instance;
}

IcdWidgetManager::IcdWidgetManager(QObject *parent) : IcdActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("IcdWidgetManager");
    Utils::Log::addMessage(this, "Instance created");
}

void IcdWidgetManager::updateContext(Core::IContext *object)
{
    IcdCentralWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<IcdCentralWidget *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;   // keep trace of the last active view (we need it in dialogs)
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        IcdActionHandler::setCurrentView(view);
    }
}

IcdCentralWidget *IcdWidgetManager::currentView() const
{
    return IcdActionHandler::m_CurrentView;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
IcdActionHandler::IcdActionHandler(QObject *parent) :
        QObject(parent),
        aRecreateDatabase(0),
        aShowDatabaseInformations(0),
        aSearchByLabel(0),
        aSearchByCode(0),
        gSearchMethod(0), gModes(0),
        aSelectorSimpleMode(0), aSelectorFullMode(0),
        aCollectionModelFullMode(0), aCollectionModelSimpleMode(0),
        aToggleSelector(0), aClear(0), aRemoveRow(0), aPrint(0), aPrintPreview(0),
        m_CurrentView(0)
{
    setObjectName("IcdActionHandler");
    Utils::Log::addMessage(this, "Instance created");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(ICD::Constants::C_ICD_PLUGINS);
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

//    Core::ActionContainer *menu = actionManager()->actionContainer(Constants::M_PLUGINS_ICD);
//    if (!menu) {
//        menu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_ICD);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_VIEWS);
//        menu->setTranslations(DrugsWidget::Constants::DRUGSMENU_TEXT);
//    }
//    Q_ASSERT(menu);
//#ifdef FREEDIAMS
//    actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, DrugsWidget::Constants::G_PLUGINS_DRUGS);
//#else
//    actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_DRUGS);
//#endif

    // Create ICD10 database
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    a = aRecreateDatabase = new QAction(this);
    a->setObjectName("aRecreateDatabase");
    cmd = actionManager()->registerAction(a, Constants::A_RECREATE_ICD_DB, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Constants::RECREATE_DATABASE_TEXT, Constants::RECREATE_DATABASE_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    connect(a, SIGNAL(triggered()), this, SLOT(recreateDatabase()));

    // Show Databases informations
    a = aShowDatabaseInformations = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_DATABASE_INFOS, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Constants::DATABASE_INFOS_TEXT, Constants::DATABASE_INFOS_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    cmd->retranslate();
    hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    connect(aShowDatabaseInformations,SIGNAL(triggered()), this, SLOT(showDatabaseInformations()));

    // Search method menu
    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_EDIT);
    Core::ActionContainer *searchmenu = actionManager()->actionContainer(Constants::M_ICD_SEARCH);
    if (!searchmenu) {
        searchmenu = actionManager()->createMenu(Constants::M_ICD_SEARCH);
        searchmenu->appendGroup(Constants::G_ICD_SEARCH);
        searchmenu->setTranslations(Constants::SEARCHMENU_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
        menu->addMenu(searchmenu, Core::Constants::G_EDIT_FIND);
    }
    Q_ASSERT(searchmenu);

    gSearchMethod = new QActionGroup(this);
    a = aSearchByLabel = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Constants::I_SEARCH_LABEL));
    cmd = actionManager()->registerAction(a, Constants::A_SEARCH_LABEL, ctx);
    cmd->setTranslations(Constants::SEARCHLABEL_TEXT, Constants::SEARCHLABEL_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, Constants::M_ICD_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchByCode = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Constants::I_SEARCH_CODE));
    cmd = actionManager()->registerAction(a, Constants::A_SEARCH_CODE, ctx);
    cmd->setTranslations(Constants::SEARCHCODE_TEXT, Constants::SEARCHCODE_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, Constants::M_ICD_SEARCH);
    gSearchMethod->addAction(a);
    connect(gSearchMethod,SIGNAL(triggered(QAction*)),this,SLOT(searchActionChanged(QAction*)));

    // Modes
    Core::ActionContainer *modesmenu = actionManager()->actionContainer(Constants::M_ICD_MODES);
    if (!modesmenu) {
        modesmenu = actionManager()->createMenu(Constants::M_ICD_MODES);
        modesmenu->appendGroup(Constants::G_ICD_SELECTORMODE);
        modesmenu->appendGroup(Constants::G_ICD_COLLECTIONMODE);
        modesmenu->setTranslations(Constants::MODESMENU_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
        menu->addMenu(modesmenu, Core::Constants::G_EDIT_OTHER);
    }
    Q_ASSERT(modesmenu);

    gModes = new QActionGroup(this);
    a = aSelectorSimpleMode = new QAction(this);
    a->setObjectName("aSelectorSimpleMode");
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, Constants::A_SELECTOR_SIMPLEMODE, ctx);
    cmd->setTranslations(Constants::SELECTORSIMPLEMODE_TEXT, Constants::SELECTORSIMPLEMODE_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    modesmenu->addAction(cmd, Constants::M_ICD_MODES);
    gModes->addAction(a);

    a = aSelectorFullMode = new QAction(this);
    a->setObjectName("aSelectorFullMode");
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, Constants::A_SELECTOR_FULLMODE, ctx);
    cmd->setTranslations(Constants::SELECTORFULLMODE_TEXT, Constants::SELECTORFULLMODE_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    modesmenu->addAction(cmd, Constants::M_ICD_MODES);
    gModes->addAction(a);

    a = aCollectionModelSimpleMode = new QAction(this);
    a->setObjectName("aCollectionModelSimpleMode");
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, Constants::A_COLLECTION_SIMPLEMODE, ctx);
    cmd->setTranslations(Constants::COLLECTIONSIMPLEMODE_TEXT, Constants::COLLECTIONSIMPLEMODE_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    modesmenu->addAction(cmd, Constants::M_ICD_MODES);
    gModes->addAction(a);

    a = aCollectionModelFullMode = new QAction(this);
    a->setObjectName("aCollectionModelFullMode");
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, Constants::A_COLLECTION_FULLMODE, ctx);
    cmd->setTranslations(Constants::COLLECTIONFULLMODE_TEXT, Constants::COLLECTIONFULLMODE_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
    modesmenu->addAction(cmd, Constants::M_ICD_MODES);
    gModes->addAction(a);
    connect(gModes, SIGNAL(triggered(QAction*)), this, SLOT(modeActionChanged(QAction*)));

    // File Open/Save/Print/printPreview
    a = aToggleSelector = new QAction(this);
    a->setObjectName("aToggleSelector");
    a->setIcon(th->icon(Constants::I_TOGGLEICDSELECTOR));
    cmd = actionManager()->registerAction(a, Constants::A_TOGGLE_ICDSELECTOR, ctx);
    cmd->setTranslations(Constants::TOGGLEICDSELECTOR_TEXT, Constants::TOGGLEICDSELECTOR_TEXT, Constants::ICDCONSTANTS_TR_CONTEXT);
//    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_VIEWS);
    connect(a, SIGNAL(triggered()), this, SLOT(toggleSelector()));

    a = aClear = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
//    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    a = aRemoveRow = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_REMOVE, ctx);
    cmd->setTranslations(Trans::Constants::LISTREMOVE_TEXT);
//    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    Q_ASSERT(fmenu);
    if (!fmenu)
        return;
    a = aPrint = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONPRINT));
    cmd = actionManager()->registerAction(a, Constants::A_PRINT_COLLECTION, ctx);
    cmd->setTranslations(Constants::PRINTCOLLECTION_TEXT, "", Constants::ICDCONSTANTS_TR_CONTEXT);
#ifdef FREEICD
    cmd->setKeySequence(QKeySequence::Print);
#else
    cmd->setKeySequence(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
#endif
    cmd->retranslate();
    if (fmenu) {
        fmenu->addAction(cmd, Core::Constants::G_FILE_PRINT);
    }
    connect(aPrint,SIGNAL(triggered()), this, SLOT(print()));

    a = aPrintPreview = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONPRINTPREVIEW));
    //    a->setShortcut(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd = actionManager()->registerAction(a, Core::Constants::A_FILE_PRINTPREVIEW, ctx);
    cmd->setTranslations(Trans::Constants::PRINTPREVIEW_TEXT, Trans::Constants::PRINTPREVIEW_TEXT);
    cmd->retranslate();
    if (fmenu) {
        fmenu->addAction(cmd, Core::Constants::G_FILE_PRINT);
    }
    connect(aPrintPreview,SIGNAL(triggered()), this, SLOT(printPreview()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

void IcdActionHandler::setCurrentView(IcdCentralWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        Utils::Log::addError(this, "setCurrentView : no view", __FILE__, __LINE__);
        return;
    }
    //    qWarning() << "IcdActionHandler::setCurrentView(IcdContextualWidget *view)";

    // disconnect old view
    if (m_CurrentView) {
        if (view == m_CurrentView.data())
            return;
//        m_CurrentView->disconnect();
//        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//                   this, SLOT(listViewItemChanged()));
//        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//                   this, SLOT(drugsModelChanged()));
//        m_CurrentView->drugSelector()->disconnectFilter();
    }
    m_CurrentView = view;

    // reconnect some actions
//    m_CurrentView->createConnections();
//    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//            this, SLOT(listViewItemChanged()));
//    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//            this, SLOT(drugsModelChanged()));
//    m_CurrentView->drugSelector()->connectFilter();
    updateActions();
}

void IcdActionHandler::updateActions()
{
    if (!m_CurrentView)
        return;

    if (m_CurrentView->selectorMode() == IcdCentralWidget::SelectorSimpleMode) {
        aSelectorSimpleMode->setChecked(true);
        aSelectorFullMode->setChecked(false);
    } else {
        aSelectorSimpleMode->setChecked(false);
        aSelectorFullMode->setChecked(true);
    }

    if (m_CurrentView->collectionMode() == IcdCentralWidget::CollectionSimpleMode) {
        aCollectionModelSimpleMode->setChecked(true);
        aCollectionModelFullMode->setChecked(false);
    } else {
        aCollectionModelSimpleMode->setChecked(false);
        aCollectionModelFullMode->setChecked(true);
    }
}

void IcdActionHandler::recreateDatabase()
{
    if (!m_Downloader)
        m_Downloader = new IcdDownloader(this);
    m_Downloader->createDatabase();
    connect(m_Downloader, SIGNAL(processEnded()), m_Downloader, SLOT(deleteLater()));
}

void IcdActionHandler::showDatabaseInformations()
{
    QDialog dlg(qApp->activeWindow(), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();

    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}

void IcdActionHandler::searchActionChanged(QAction *a)
{
}

void IcdActionHandler::modeActionChanged(QAction *a)
{
    if (!m_CurrentView)
        return;
    if (a == aSelectorSimpleMode) {
        m_CurrentView->setSelectorMode(IcdCentralWidget::SelectorSimpleMode);
    } else if (a == aSelectorFullMode) {
        m_CurrentView->setSelectorMode(IcdCentralWidget::SelectorSimpleMode);
    } else if (a == aCollectionModelSimpleMode) {
        m_CurrentView->setCollectionMode(IcdCentralWidget::CollectionSimpleMode);
    } else if (a == aCollectionModelFullMode) {
        m_CurrentView->setCollectionMode(IcdCentralWidget::CollectionFullMode);
    }
}

void IcdActionHandler::toggleSelector()
{
    if (m_CurrentView)
        m_CurrentView->toggleSelector();
}

void IcdActionHandler::clear()
{
    if (m_CurrentView)
        m_CurrentView->clear();
}

void IcdActionHandler::removeItem()
{
    if (m_CurrentView)
        m_CurrentView->removeItem();
}

void IcdActionHandler::print()
{
    if (m_CurrentView)
        m_CurrentView->print();
}
