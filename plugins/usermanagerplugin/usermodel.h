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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERMODEL_H
#define USERMODEL_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <usermanagerplugin/constants.h>

#include <coreplugin/iuser.h>

#include <QSqlTableModel>
#include <QAbstractListModel>
#include <QHash>
#include <QStringList>


/**
 * \file usermodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 15 May 2011
*/

namespace Print {
class PrinterPreviewer;
class TextDocumentExtra;
}


namespace UserPlugin {
class UserManagerPlugin;
class UserModel;

namespace Internal {
class UserModelPrivate;

class UserModelWrapper : public Core::IUser
{
    Q_OBJECT
public:
    UserModelWrapper(UserModel *model);
    ~UserModelWrapper();

    // IPatient interface
    void clear() {}
    bool has(const int ref) const {return (ref>=0 && ref<Core::IUser::NumberOfColumns);}
    bool hasCurrentUser() const;

    QVariant value(const int ref) const;
    bool setValue(const int ref, const QVariant &value);

    /** \todo Is this needed in freemedforms ? */
    QString toXml() const {return QString();}
    bool fromXml(const QString &) {return true;}

    bool saveChanges();

private Q_SLOTS:
    void newUserConnected(const QString &uid);

private:
    UserModel *m_Model;
};
}  // End Internal



class USER_EXPORT UserModel : public QSqlTableModel
{
    Q_OBJECT
    friend class UserPlugin::UserManagerPlugin;

protected:
    UserModel(QObject *parent);

public:
    static UserModel *instance(QObject *parent = 0);
    ~UserModel();

    bool isCorrectLogin(const QString &clearLog, const QString &clearPassword);

    bool setCurrentUser(const QString &clearLog, const QString &clearPassword, bool refreshCache = false);
    bool setCurrentUserIsServerManager();

    bool hasCurrentUser();
    QModelIndex currentUserIndex() const;

    bool hasUserToSave();

    virtual QSqlDatabase database() const;

    virtual void clear();
    virtual void refresh();

    virtual int columnCount(const QModelIndex & = QModelIndex());

    QVariant currentUserData(const int column) const;
    virtual QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool setPaper(const QString &uuid, const int ref, Print::TextDocumentExtra *extra);

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual void revertRow(int row);

    virtual void setSort(int column, Qt::SortOrder order);
    virtual void sort(int column, Qt::SortOrder order);
    virtual void setTable(const QString &tableName);
    virtual void setFilter(const QHash<int,QString> &conditions);
    virtual void setFilter(const QString &filter);

    QString tableName() const;

    QList<int> practionnerLkIds(const QString &uid);
    int practionnerLkId(const QString &uid);

    int numberOfUsersInMemory();

    void warn();

public Q_SLOTS:
    virtual bool submitAll();
    virtual bool submitUser(const QString &uuid);
    virtual bool submitRow(const int row);
    virtual bool revertAll();

Q_SIGNALS:
    void memoryUsageChanged() const;
    void userAboutToConnect(const QString &uuid) const;
    void userConnected(const QString &uuid) const;
    void userAboutToDisconnect(const QString &uuid) const;
    void userDisconnected(const QString &uuid) const;
    void userDocumentsChanged() const;

protected:
    void emitUserConnected() const;

private Q_SLOTS:
    void updateUserPreferences();

private:
    Internal::UserModelPrivate *d;
    static UserModel *m_Instance;
};

}  // End UserPlugin


#endif // End USERMODEL_H
