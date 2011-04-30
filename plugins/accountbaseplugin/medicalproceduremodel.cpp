/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "medicalproceduremodel.h"
#include "accountbase.h"
#include "constants.h"
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QUuid>

using namespace AccountDB;
using namespace Trans::ConstantTranslations;

enum {WarnFilter=true};

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline AccountDB::AccountBase *accountBase() {return AccountDB::AccountBase::instance();}
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }

namespace AccountDB {
namespace Internal {

class MedicalProcedureModelPrivate
{
public:
    MedicalProcedureModelPrivate(MedicalProcedureModel *parent) : m_SqlTable(0), 
                                                                  m_IsDirty(false),
                                                                  m_UserUid(user()->value(Core::IUser::Uuid).toString()),
                                                                  q(parent)
    {
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        setTable();
        //refreshFilter();
    }
    ~MedicalProcedureModelPrivate () {}
    
    void setTable(){
          m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_MedicalProcedure));

    }
    
    /*void refreshFilter()
    {
        if (!m_SqlTable)
            return;
        QHash<int, QString> where;
        where.insert(AccountDB::Constants::MP_USER_UID, QString("='%1'").arg(m_UserUid));
        if (!m_TypeFilter.isEmpty()) {
            where.insert(AccountDB::Constants::MP_TYPE, QString("='%1'").arg(m_TypeFilter));
        }
        if (!m_nameFilter.isEmpty() )
        {
        	  where.insert(AccountDB::Constants::MP_NAME, QString("='%1'").arg(m_nameFilter));
            } 
            
        m_SqlTable->setFilter(accountBase()->getWhereClause(Constants::Table_MedicalProcedure, where));
        if (WarnFilter){
            qWarning() << m_SqlTable->filter() << __FILE__ << __LINE__;
            }
        //q->reset();
     }*/

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;
    QString m_UserUid, m_TypeFilter ,m_nameFilter ;

private:
    MedicalProcedureModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



MedicalProcedureModel::MedicalProcedureModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::MedicalProcedureModelPrivate(this))
{
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->select();
}

MedicalProcedureModel::~MedicalProcedureModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int MedicalProcedureModel::rowCount(const QModelIndex &parent) const
{ 
    int rows = 0;
    d->m_SqlTable->setFilter("");
    d->m_SqlTable->select();
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int MedicalProcedureModel::rowCountWithFilter(const QModelIndex &parent, const QString & filter) {
    int rows = 0;
    //d->m_SqlTable->setFilter(filter);
    //d->m_SqlTable->select();
    qDebug() << __FILE__ << QString::number(__LINE__) << "d->m_SqlTable->filter()  =" << d->m_SqlTable->filter() ;
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int MedicalProcedureModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

QStringList MedicalProcedureModel::distinctAvailableType() const
{
    QStringList toReturn;
    QSqlQuery query(accountBase()->database());
    query.exec(accountBase()->selectDistinct(AccountDB::Constants::Table_MedicalProcedure, AccountDB::Constants::MP_TYPE));
    if (query.isActive()) {
        while (query.next()) {
            toReturn << query.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    query.finish();
    return toReturn;
}

void MedicalProcedureModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::MP_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(AccountBase::instance()->getWhereClause(Constants::Table_MedicalProcedure, where));
}

QVariant MedicalProcedureModel::data(const QModelIndex &index, int role) const
{   
    return d->m_SqlTable->data(index, role);
}

QVariant MedicalProcedureModel::dataWithFilter(const QModelIndex &index, int role , const QString & filter) const
{   
    d->m_SqlTable->setFilter(filter);
    return d->m_SqlTable->data(index, role);
}

bool MedicalProcedureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant MedicalProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->m_SqlTable->headerData(section,orientation,role);
}

bool MedicalProcedureModel::setHeaderData(int section,Qt::Orientation orientation,
                                          QVariant & value,int role ){
    return d->m_SqlTable->setHeaderData(section, orientation,value,role )  ;                                   
}

bool MedicalProcedureModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ok = true;
    for(int i = 0 ; i < count ; ++i) {
//        QSqlQuery query(accountBase()->database());
//        query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_MedicalProcedure));
//        query.bindValue(AccountDB::Constants::MP_ABSTRACT, QVariant());
//        query.bindValue(AccountDB::Constants::MP_AMOUNT, QVariant());
//        query.bindValue(AccountDB::Constants::MP_DATE, QVariant());
//        query.bindValue(AccountDB::Constants::MP_ID, QVariant());
//        query.bindValue(AccountDB::Constants::MP_NAME, QVariant());
//        query.bindValue(AccountDB::Constants::MP_REIMBOURSEMENT, QVariant());
//        query.bindValue(AccountDB::Constants::MP_TYPE, QVariant());
//        query.bindValue(AccountDB::Constants::MP_UID, QUuid::createUuid().toString());
//        query.bindValue(AccountDB::Constants::MP_USER_UID, user()->value(Core::IUser::Uuid));
//        if (!query.exec()) {
//            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
//        }
//        query.finish();

        if (!d->m_SqlTable->insertRow(row+i, parent)) {
            qWarning() << d->m_SqlTable->database().lastError().text();
            ok = false;
        } else {
            // Set CurrentUser UUID
            QModelIndex userUid = d->m_SqlTable->index(row+i, Constants::MP_USER_UID, parent);
            if (!d->m_SqlTable->setData(userUid, user()->value(Core::IUser::Uuid))) {
                qWarning() << d->m_SqlTable->database().lastError().text();
                ok = false;
            }
            // Create MP UUID
            QModelIndex mpUid = d->m_SqlTable->index(row+i, Constants::MP_UID, parent);
            if (!d->m_SqlTable->setData(mpUid, QUuid::createUuid().toString())) {
                qWarning() << d->m_SqlTable->database().lastError().text();
                ok = false;
            }
        }

    }
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
//    d->m_SqlTable->select();
//    reset();
    return ok;
}

bool MedicalProcedureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

void MedicalProcedureModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    //d->m_SqlTable->select();
}

QString MedicalProcedureModel::filter(){
    return d->m_SqlTable->filter();
}

bool MedicalProcedureModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void MedicalProcedureModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool MedicalProcedureModel::isDirty() const
{
    return d->m_IsDirty;
}

QSqlError MedicalProcedureModel::lastError(){
    return d->m_SqlTable->lastError();
}

void MedicalProcedureModel::clear(){
    d->m_SqlTable->clear();
}
