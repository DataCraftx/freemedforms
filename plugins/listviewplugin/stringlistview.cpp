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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class Views::StringListView
  \brief Provides a stringlist view to use with tkUserViewer. For eg with the specialties, qualifications... of users.
  - Property stringList is used for QDataWidgetMapper.
  - Holds the mecanism for add, remove, move up and down for all items using a contextmenu.
  - Set and get the model stringList using getStringList() and setStringList().

  - You can use it with the tkStringListModel and set items to be checkable. Then populate the stringlist with the
  setStringList() and manage checked items with property checkedStringList. The getCheckedStringList() and
  setItemsCheckable() only work if the model of the view is a tkStringListModel.
*/

#include "stringlistview.h"
#include "stringlistmodel.h"

#include <QAction>
#include <QMenu>

#include <QDebug>

using namespace Views;

StringListView::StringListView( QWidget * parent )
        : ListView(parent)
{
}

StringListView::~StringListView()
{
}

QVariant StringListView::getStringList() const
{
    QStringListModel *model = static_cast<QStringListModel*>(this->model());
    if (model) {
        return model->stringList();
    }
    return QVariant();
}

void StringListView::setStringList(const QVariant &list)
{
    QStringListModel *model = static_cast<QStringListModel*>(this->model());
    if (model)
        model->setStringList(list.toStringList());
}

QVariant StringListView::getCheckedStringList() const
{
    Q_ASSERT_X( static_cast<StringListModel*>(this->model()), "StringListView::getCheckedStringList()",
                "This member can only be used if the model is a tkStringListModel.");
    StringListModel *m = static_cast<StringListModel*>(this->model());
    if (!m)
        return QVariant();
    return m->getCheckedItems();
}

void StringListView::setCheckedStringList(const QVariant &list)
{
    Q_ASSERT_X( static_cast<StringListModel*>(this->model()), "StringListView::setCheckedStringList()",
                "This member can only be used if the model is a tkStringListModel.");
    StringListModel * m = static_cast<StringListModel*>(this->model());
    if (!m)
        return ;
    m->setCheckedItems(list.toStringList());
}

void StringListView::setItemsCheckable(bool state)
{
    StringListModel *m = static_cast<StringListModel*>(this->model());
    if (m)
        m->setCheckable(state);
}
