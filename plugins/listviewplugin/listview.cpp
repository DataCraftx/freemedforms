/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
/**
  * \class Views::ListView
  * \brief This widget shows a QListView with some buttons and a context menu.
  * Actions inside the contextual menu and the toolbar can be setted using
    AvailableActions param and setActions().\n
  * You can reimplement addItem(), removeItem() and on_edit_triggered()
    that are called by buttons and menu (add , remove). \n
  * The getContextMenu() is automatically called when the contextMenu is requiered.\n
    You can reimplement getContextMenu() in order to use your own contextMenu.\n
    Remember that the poped menu will be deleted after execution. \n
  \todo There is a problem when including this widget into a QDataWidgetMapper, when this widget loses focus, datas are not retreived.
*/

#include "listview.h"
#include "stringlistmodel.h"
#include "constants.h"
#include "viewmanager.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QToolButton>
#include <QToolBar>

using namespace Views;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


/////////////////////////////////////////////////////////////////////////// List View
namespace Views {
namespace Internal {

class ListViewPrivate
{
public:
    ListViewPrivate(QWidget *parent, Constants::AvailableActions actions) :
            m_Parent(parent),
            m_Actions(actions),
            m_Context(0),
            m_ExtView(0)
    {
    }

    ~ListViewPrivate()
    {
        if (m_ExtView)
            delete m_ExtView;
        m_ExtView = 0;
    }

    void calculateContext()
    {
        m_Context->clearContext();
        if (m_Actions & Constants::MoveUpDown)
            m_Context->addContext(Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(Constants::C_BASIC_MOVE));

        if (m_Actions & Constants::AddRemove)
            m_Context->addContext(Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(Constants::C_BASIC_ADDREMOVE));
    }

public:
    QWidget *m_Parent;
    Constants::AvailableActions m_Actions;
    ViewContext *m_Context;
    QToolBar *m_ToolBar;
    QString m_ContextName;
    ExtendedView *m_ExtView;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
ListView::ListView(QWidget *parent, Constants::AvailableActions actions) :
        QListView(parent),
        d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("ListView_"+QString::number(handler));
    setProperty(Constants::HIDDEN_ID, "xx");
    d = new Internal::ListViewPrivate(this, actions);

    // Create the Manager instance and context
    d->m_Context = new ViewContext(this);
    d->calculateContext();
    contextManager()->addContextObject(d->m_Context);

    // Create the extension of the view
    d->m_ExtView = new ExtendedView(this, actions);
}

ListView::~ListView()
{
    contextManager()->removeContextObject(d->m_Context);
}

void ListView::setActions(Constants::AvailableActions actions)
{
    d->m_Actions = actions;
    d->calculateContext();
    d->m_ExtView->setActions(actions);
}

void ListView::setCommands(const QStringList &commandsUid)
{
    d->m_Actions = 0;
    d->calculateContext();
    d->m_ExtView->setCommands(commandsUid);
}

void ListView::addContext(const int id)
{
    d->m_Context->addContext(id);
}

void ListView::addContexts(const QList<int> &id)
{
    for(int i = 0; i < id.count(); ++i) {
        d->m_Context->addContext(id.at(i));
    }
}

void ListView::hideButtons() const
{
    d->m_ExtView->hideButtons();
}

void ListView::showButtons()
{
    d->m_ExtView->showButtons();
}

void ListView::useContextMenu(bool state)
{
    d->m_ExtView->useContextMenu(state);
}

void ListView::addItem()
{
    setFocus();
    d->m_ExtView->addItem();
}

void ListView::removeItem()
{
    setFocus();
    d->m_ExtView->removeItem();
}

void ListView::moveDown()
{
    setFocus();
    d->m_ExtView->moveDown();
}

void ListView::moveUp()
{
    setFocus();
    d->m_ExtView->moveUp();
}

