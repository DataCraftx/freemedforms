/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugdruginteractiontablemodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QFont>
#include <QLocale>
#include <QDate>
#include <QColor>
#include <QSqlTableModel>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::Internal::DDIDatabase &ddiBase() {return DDI::DDICore::instance()->database();}

namespace DDI {
namespace Internal {
class DrugDrugInteractionTableModelPrivate
{
public:
    DrugDrugInteractionTableModelPrivate(DrugDrugInteractionTableModel *parent) :
        _sql(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DrugDrugInteractionTableModelPrivate()
    {
    }

    int modelColumnToSqlColumn(const int modelColumn)
    {
        int sql = -1;
        switch (modelColumn) {
        case DrugDrugInteractionTableModel::Id: sql = Constants::DDI_ID; break;
        case DrugDrugInteractionTableModel::Uid: sql = Constants::DDI_UID; break;
        case DrugDrugInteractionTableModel::IsValid: sql = Constants::DDI_ISVALID; break;
        case DrugDrugInteractionTableModel::FirstInteractorUid: sql = Constants::DDI_FIRSTINTERACTORUID; break;
        case DrugDrugInteractionTableModel::SecondInteractorUid: sql = Constants::DDI_SECONDINTERACTORUID; break;
        case DrugDrugInteractionTableModel::IsReviewed: sql = Constants::DDI_ISREVIEWED; break;
        case DrugDrugInteractionTableModel::LevelCode: sql = Constants::DDI_LEVELCODE; break;
        //case DrugDrugInteractionTableModel::LevelName: sql = Constants::INTERACTOR_ISVALID; break;
        //case DrugDrugInteractionTableModel::LevelComboIndex: sql = Constants::INTERACTOR_ISVALID; break;
        case DrugDrugInteractionTableModel::DateCreation: sql = Constants::DDI_DATECREATION; break;
        case DrugDrugInteractionTableModel::DateLastUpdate: sql = Constants::DDI_DATELASTUPDATE; break;
        case DrugDrugInteractionTableModel::RiskFr: sql = Constants::DDI_RISKFR; break;
        case DrugDrugInteractionTableModel::RiskEn: sql = Constants::DDI_RISKEN; break;
        case DrugDrugInteractionTableModel::ManagementFr: sql = Constants::DDI_MANAGEMENTFR; break;
        case DrugDrugInteractionTableModel::ManagementEn: sql = Constants::DDI_MANAGEMENTEN; break;
        case DrugDrugInteractionTableModel::ReviewersStringList: sql = Constants::DDI_REVIEWERSSTRINGLIST; break;
        case DrugDrugInteractionTableModel::Source: sql = Constants::DDI_SOURCE; break;
        case DrugDrugInteractionTableModel::Comment: sql = Constants::DDI_COMMENT; break;
        case DrugDrugInteractionTableModel::InternalUid: sql = Constants::DDI_INTERNALUID; break;
        case DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds: sql = Constants::DDI_FIRSTINTERACTORROUTEOFADMINISTRATIONIDS; break;
        case DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds: sql = Constants::DDI_SECONDINTERACTORROUTEOFADMINISTRATIONIDS; break;
        case DrugDrugInteractionTableModel::FirstDoseUseFrom: sql = Constants::DDI_FIRSTDOSEUSEFROM; break;
        case DrugDrugInteractionTableModel::FirstDoseUsesTo: sql = Constants::DDI_FIRSTDOSEUSESTO; break;
        case DrugDrugInteractionTableModel::FirstDoseFromValue: sql = Constants::DDI_FIRSTDOSEFROMVALUE; break;
        case DrugDrugInteractionTableModel::FirstDoseFromUnits: sql = Constants::DDI_FIRSTDOSEFROMUNITS; break;
        case DrugDrugInteractionTableModel::FirstDoseFromRepartition: sql = Constants::DDI_FIRSTDOSEFROMREPARTITION; break;
        case DrugDrugInteractionTableModel::FirstDoseToValue: sql = Constants::DDI_FIRSTDOSETOVALUE; break;
        case DrugDrugInteractionTableModel::FirstDoseToUnits: sql = Constants::DDI_FIRSTDOSETOUNITS; break;
        case DrugDrugInteractionTableModel::FirstDoseToRepartition: sql = Constants::DDI_FIRSTDOSETOREPARTITION; break;
        case DrugDrugInteractionTableModel::SecondDoseUseFrom: sql = Constants::DDI_SECONDDOSEUSEFROM; break;
        case DrugDrugInteractionTableModel::SecondDoseUsesTo: sql = Constants::DDI_SECONDDOSEUSESTO; break;
        case DrugDrugInteractionTableModel::SecondDoseFromValue: sql = Constants::DDI_SECONDDOSEFROMVALUE; break;
        case DrugDrugInteractionTableModel::SecondDoseFromUnits: sql = Constants::DDI_SECONDDOSEFROMUNITS; break;
        case DrugDrugInteractionTableModel::SecondDoseFromRepartition: sql = Constants::DDI_SECONDDOSEFROMREPARTITION; break;
        case DrugDrugInteractionTableModel::SecondDoseToValue: sql = Constants::DDI_SECONDDOSETOVALUE; break;
        case DrugDrugInteractionTableModel::SecondDoseToUnits: sql = Constants::DDI_SECONDDOSETOUNITS; break;
        case DrugDrugInteractionTableModel::SecondDoseToRepartition: sql = Constants::DDI_SECONDDOSETOREPARTITION; break;
        case DrugDrugInteractionTableModel::PMIDStringList: sql = Constants::DDI_PMIDSTRINGLIST; break;
        };
        return sql;
    }

public:
    QSqlTableModel *_sql;

private:
    DrugDrugInteractionTableModel *q;
};
}  // End namespace Internal
}  // End namespace DDI


DrugDrugInteractionTableModel::DrugDrugInteractionTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::DrugDrugInteractionTableModelPrivate(this))
{
    setObjectName("DrugDrugInteractionTableModel");
    d->_sql = new QSqlTableModel(this, ddiBase().database());
    d->_sql->setTable(ddiBase().table(Constants::Table_DDI));
}

DrugDrugInteractionTableModel::~DrugDrugInteractionTableModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Initialize the model (fetch all interactors from database). */
bool DrugDrugInteractionTableModel::initialize()
{
    // Select
    d->_sql->select();

    // Fetch all data in the source model
    while (d->_sql->canFetchMore(index(0, d->_sql->rowCount())))
        d->_sql->fetchMore(index(0, d->_sql->rowCount()));

    return true;
}

int DrugDrugInteractionTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int DrugDrugInteractionTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void DrugDrugInteractionTableModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool DrugDrugInteractionTableModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

QVariant DrugDrugInteractionTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), d->modelColumnToSqlColumn(index.column()));
        return d->_sql->data(sqlIndex, role);
//    } else if (role == Qt::FontRole) {
//        QModelIndex sqlIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ISCLASS);
//        if (d->_sql->data(sqlIndex).toBool()) {
//            QFont bold;
//            bold.setBold(true);
//            return bold;
//        }
    } else if (role==Qt::ForegroundRole) {
        QModelIndex isRev = d->_sql->index(index.row(), Constants::DDI_ISREVIEWED);
        if (!d->_sql->data(isRev).toBool()) {
            return QColor(50,250,50,150);
        }
    }
    return QVariant();
}

bool DrugDrugInteractionTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    int sql = d->modelColumnToSqlColumn(index.column());
    if (role == Qt::EditRole) {
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        switch (index.column()) {
        case IsValid:
        case IsReviewed:
            ok = d->_sql->setData(sqlIndex, value.toBool()?1:0, role);
            break;
        default: ok = d->_sql->setData(sqlIndex, value, role); break;
        }
        return ok;
    }
    return false;
}

bool DrugDrugInteractionTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return d->_sql->insertRows(row, count, parent);
}

bool DrugDrugInteractionTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    for(int i = 0; i < count; ++i)
        d->_sql->setData(d->_sql->index(row+i, IsValid), 0);
    endRemoveRows();
    return true;
}

//QVariant DrugDrugInteractionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//        switch (section) {
//        case Id: return tr("Id");
//        case Uuid: return tr("Uuid");
//        case IsValid: return tr("Is valid");
//        case IsInteractingClass: return tr("Is interaction class");
//        case IsReviewed: return tr("Is reviewed");
//        case IsAutoFound: return tr("Is auto-found");
//        case DoNotWarnDuplicated: return tr("DoNotWarnDuplicated");
//        case TranslatedLabel: return tr("Translated label");
//        case EnLabel: return tr("English label");
//        case FrLabel: return tr("French label");
//        case DeLabel: return tr("Deustch label");
//        case ClassInformationFr: return tr("Class information (french)");
//        case ClassInformationEn: return tr("Class information (english)");
//        case ClassInformationDe: return tr("Class information (deustch)");
//        case ATCCodeStringList: return tr("ATC codes");
//        case DateOfCreation: return tr("Date of creation");
//        case DateLastUpdate: return tr("Date of update");
//        case DateReview: return tr("Date of review");
//        case PMIDStringList: return tr("PMID");
//        case ChildrenUuid: return tr("Children");
//        case Reference: return tr("Reference");
//        case Comment: return tr("Comment");
//        }
//    }
//    return QVariant();
//}

Qt::ItemFlags DrugDrugInteractionTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return f;
}

QStringList DrugDrugInteractionTableModel::units()
{
    QStringList l;
    l << "mg";
    l << "µg";
    l << "g";
    l << "UI";
    l << "ml";
    l << "dl";
    l << "cl";
    l << "ml";
    return l;
}

QStringList DrugDrugInteractionTableModel::repartitions()
{
    QStringList l;
    l << tkTr(Trans::Constants::MINUTES);
    l << tkTr(Trans::Constants::HOURS);
    l << tkTr(Trans::Constants::DAYS);
    l << tkTr(Trans::Constants::WEEKS);
    return l;
}

QString DrugDrugInteractionTableModel::unit(int index)
{
    const QStringList &l = units();
    if (index < l.count() && index >= 0)
        return l.at(index);
    return QString();
}

QString DrugDrugInteractionTableModel::repartition(int index)
{
    const QStringList &l = repartitions();
    if (index < l.count() && index >= 0)
        return l.at(index);
    return QString();
}


/** Returns the number of unreviewed DDI::DrugInteractor from the database. */
int DrugDrugInteractionTableModel::numberOfUnreviewed() const
{
    // directly ask the database instead of screening the model
    QHash<int, QString> where;
    where.insert(Constants::DDI_ISREVIEWED, "=0");
    return ddiBase().count(Constants::Table_DDI, Constants::DDI_ID, ddiBase().getWhereClause(Constants::Table_DDI, where));
}

/** Submit changes directly to the database */
bool DrugDrugInteractionTableModel::submit()
{
    bool ok = d->_sql->submitAll();
    if (ok)
        d->_sql->database().commit();
    return ok;
}