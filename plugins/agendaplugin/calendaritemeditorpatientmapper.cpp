/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "calendaritemeditorpatientmapper.h"
#include "calendaritemmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <calendar/calendar_item.h>
#include <calendar/calendar_people.h>
#include <calendar/abstract_calendar_model.h>
#include <translationutils/constanttranslations.h>

#include "ui_calendaritemeditorpatientmapper.h"

#include <QStyledItemDelegate>
#include <QAbstractTableModel>
#include <QDebug>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace {

class TreeItemDelegate : public QStyledItemDelegate
{
public:
    TreeItemDelegate(QObject *parent) : QStyledItemDelegate(parent), m_FancyColumn(-1)
    {
    }

    void setModel(QAbstractItemModel *model) {m_Model = model;}
    void setFancyColumn(int col) {m_FancyColumn=col;}
    int fancyColumn() {return m_FancyColumn;}

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        if (m_FancyColumn==-1) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        if (option.state & QStyle::State_MouseOver) {
            if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
                pressedIndex = QModelIndex();
            QBrush brush = option.palette.alternateBase();
            if (index == pressedIndex)
                brush = option.palette.dark();
            painter->fillRect(option.rect, brush);
        }

        QStyledItemDelegate::paint(painter, option, index);

        if (index.column()==m_FancyColumn && option.state & QStyle::State_MouseOver) {
            QIcon icon;
            if (option.state & QStyle::State_Selected) {
                icon = theme()->icon(Core::Constants::ICONCLOSELIGHT);
            } else {
                icon = theme()->icon(Core::Constants::ICONCLOSEDARK);
            }

            QRect iconRect(option.rect.right() - option.rect.height(),
                           option.rect.top(),
                           option.rect.height(),
                           option.rect.height());

            icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
        }
    }

public:
    mutable QModelIndex pressedIndex;
    QAbstractItemModel *m_Model;
    int m_FancyColumn;

};

}  // End anonymous namespace

CalendarItemEditorPatientMapperWidget::CalendarItemEditorPatientMapperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::CalendarItemEditorPatientMapper),
    m_ItemModel(0)
{
    ui->setupUi(this);
    TreeItemDelegate *delegate = new ::TreeItemDelegate(this);
    ui->selectedPatientView->viewport()->setAttribute(Qt::WA_Hover);
    ui->selectedPatientView->setItemDelegate(delegate);
    ui->selectedPatientView->setFrameStyle(QFrame::NoFrame);
    ui->selectedPatientView->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->selectedPatientView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->selectedPatientView->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_PeopleModel = new Calendar::CalendarPeopleModel(this);
    ui->selectedPatientView->setModel(m_PeopleModel);
    ui->selectedPatientView->header()->setStretchLastSection(false);
    ui->selectedPatientView->header()->setResizeMode(Calendar::CalendarPeopleModel::FullName, QHeaderView::Stretch);
    ui->selectedPatientView->hideColumn(Calendar::CalendarPeopleModel::Uid);
    ui->selectedPatientView->hideColumn(Calendar::CalendarPeopleModel::PeopleTypeName);
    ui->selectedPatientView->header()->setResizeMode(Calendar::CalendarPeopleModel::EmptyColumn, QHeaderView::Fixed);
    ui->selectedPatientView->setColumnWidth(Calendar::CalendarPeopleModel::EmptyColumn, 16);
    ui->selectedPatientView->header()->hide();

    delegate->setModel(m_PeopleModel);
    delegate->setFancyColumn(Calendar::CalendarPeopleModel::EmptyColumn);

    connect(ui->selectedPatientView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(ui->selectedPatientView, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));

    connect(ui->searchPatient, SIGNAL(selectedPatient(QString,QString)), this, SLOT(onPatientSelected(QString,QString)));
}

CalendarItemEditorPatientMapperWidget::~CalendarItemEditorPatientMapperWidget()
{
    delete ui;
}

void CalendarItemEditorPatientMapperWidget::setCalendarItemModel(Calendar::AbstractCalendarModel *model)
{
    m_ItemModel = model;
}

void CalendarItemEditorPatientMapperWidget::setCalendarItem(const Calendar::CalendarItem &item)
{
    clear();
    if (!m_ItemModel)
        return;

    m_PeopleModel->setPeopleList(m_ItemModel->peopleList(item));
}

void CalendarItemEditorPatientMapperWidget::clear()
{
    m_PeopleModel->clear();
    ui->searchPatient->clear();
    m_Selected.clear();
}

void CalendarItemEditorPatientMapperWidget::addPatientRow(const QString &name, const QString &uid)
{
    m_PeopleModel->addPeople(Calendar::People(Calendar::People::PeopleAttendee, name, uid));
}

void CalendarItemEditorPatientMapperWidget::removePatient(QAction *action)
{
    m_PeopleModel->removePeople(action->data().toString());
}

bool CalendarItemEditorPatientMapperWidget::submitToItem(const Calendar::CalendarItem &item)
{
    if (m_ItemModel) {
        m_ItemModel->setPeopleList(item, m_PeopleModel->peopleList());
    }
    return true;
}

void CalendarItemEditorPatientMapperWidget::onPatientSelected(const QString &name, const QString &uid)
{
    addPatientRow(name, uid);
    m_Selected.append(Calendar::People(Calendar::People::PeopleAttendee, name, uid));
    ui->searchPatient->clear();
}

void CalendarItemEditorPatientMapperWidget::handlePressed(const QModelIndex &index)
{
    if (index.column() == Calendar::CalendarPeopleModel::EmptyColumn) {
        TreeItemDelegate *delegate = static_cast<TreeItemDelegate*>(ui->selectedPatientView->itemDelegate());
        if (delegate)
            delegate->pressedIndex = index;
    }
}

void CalendarItemEditorPatientMapperWidget::handleClicked(const QModelIndex &index)
{
    if (index.column() == Calendar::CalendarPeopleModel::EmptyColumn) { // the funky button
        // remove the string from the model
        ui->selectedPatientView->model()->removeRow(index.row());
        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QWidget *vp = ui->selectedPatientView->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    }
}



CalendarItemEditorPatientMapper::CalendarItemEditorPatientMapper(QObject *parent) :
    Calendar::ICalendarItemDataWidget(parent),
    m_ItemModel(0)
{
}

CalendarItemEditorPatientMapper::~CalendarItemEditorPatientMapper()
{
}

int CalendarItemEditorPatientMapper::insertionPlace() const
{
    return Calendar::ICalendarItemDataWidget::Beginning;
}

QWidget *CalendarItemEditorPatientMapper::createWidget(QWidget *parent)
{
    if (m_Widget) {
        return m_Widget;
    }
    m_Widget = new Internal::CalendarItemEditorPatientMapperWidget(parent);
    if (m_ItemModel)
        m_Widget->setCalendarItemModel(m_ItemModel);
    return m_Widget;
}

bool CalendarItemEditorPatientMapper::setCalendarItemModel(Calendar::AbstractCalendarModel *model)
{
    m_ItemModel = model;
    if (m_Widget)
        m_Widget->setCalendarItemModel(m_ItemModel);
    return true;
}

bool CalendarItemEditorPatientMapper::setCalendarItem(const Calendar::CalendarItem &item)
{
    if (m_Widget)
        m_Widget->setCalendarItem(item);
    return true;
}

bool CalendarItemEditorPatientMapper::clear()
{
    if (m_Widget) {
        m_Widget->clear();
    }
    return true;
}

bool CalendarItemEditorPatientMapper::submitChangesToCalendarItem(const Calendar::CalendarItem &item)
{
    if (!m_Widget) {
        return false;
    }
    if (!m_ItemModel)
        return false;
    m_Widget->submitToItem(item);

    return true;
}
