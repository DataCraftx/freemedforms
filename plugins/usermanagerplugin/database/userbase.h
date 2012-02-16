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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERBASE_H
#define USERBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <usermanagerplugin/usermanager_exporter.h>

#include <QObject>

/**
 * \file userbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 14 Aug 2011
*/

namespace UserPlugin {
class UserModel;

namespace Internal {
class UserData;

class USER_EXPORT UserBase :  public QObject, public Utils::Database
{
    Q_OBJECT
    friend class UserPlugin::UserModel;

protected:
    UserBase(QObject *parent = 0);
    void onCoreDatabaseServerChanged();

public:
    // Constructor
    static UserBase *instance();
    virtual ~UserBase() {}

    // initialize
    bool initialize(Core::ISettings *settings = 0);
    bool isInitialized() const;
    bool createDatabase(const QString &connectionName, const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString &login, const QString &pass,
                        const int port,
                        CreationOption createOption
                       );
    bool checkDatabaseVersion();
    bool isNewlyCreated() const;

    bool createDefaultUser();
    bool createVirtualUser(const QString &uid, const QString &name, const QString &firstName, int title, int gender,
                           const QStringList &specialties, const QStringList &qualifications,
                           int medicalRights = 0, int adminRights = 0, int userRights = 0, int agendaRights = 0, int paramedicRights = 0,
                           QLocale::Language lang = QLocale().language());

    // retreivers
    UserData *getUserById(const QVariant &_id) const;  // not used
    UserData *getUserByLoginPassword(const QVariant &login, const QVariant &cryptedPassword) const;
    UserData *getUserByUuid(const QString &uuid) const;

    // datas retrievers
    QString getUuid(const QString &log64, const QString &cryptpass64);
    QString createNewUuid();
    QString getLogin64(const QString &uuid);
    QString getUserDynamicData(const QString &userUid, const QString &dynDataUuid);

    // savers
    bool createUser(UserData *user);
    bool saveUser(UserData *user);
    bool deleteUser(const QString &uuid);
    bool saveUserPreferences(const QString &uid, const QString &content);
    bool savePapers(UserData *user);
    bool changeUserPassword(UserData *user, const QString &clearPassword);
    bool saveUserDynamicData(const QString &userUid, const QString &dynDataUuid, const QVariant &value);

    // datas checkers
    bool checkLogin(const QString &clearLogin, const QString &clearPassword) const;
    QDateTime recordLastLogin(const QString &log, const QString &pass);

    // Linkers
    int getMaxLinkId();
    void updateMaxLinkId(const int max);

private:
    // privates retreivers
    UserData *getUser(const QHash<int, QString> &conditions) const;


private:
    static bool m_initialized;
    static QString  m_LastUuid, m_LastLogin, m_LastPass;
    static UserBase * m_Instance;
    bool m_IsNewlyCreated;
};

}  // End namespace Internal
}  // End namespace UserPlugin

#endif // USERBASE_H
