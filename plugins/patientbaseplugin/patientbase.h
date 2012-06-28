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
#ifndef PATIENTBASE_H
#define PATIENTBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <patientbaseplugin/patientbase_exporter.h>

#include <QObject>
#include <QString>
#include <QDate>

/**
 * \file patientbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 06 Aug 2011
*/

/** \todo create a sqlite db in memory to improve speed ? */

namespace Patients {
namespace Internal {
class PatientBasePlugin;
class PatientBasePrivate;
class PatientData;

class PATIENT_EXPORT PatientBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Patients::Internal::PatientBasePlugin;

protected:
    PatientBase(QObject *parent = 0);
    bool initialize();

public:
    // Constructor
    static PatientBase *instance();
    virtual ~PatientBase();

//    // initialize
//    bool initialize(Core::ISettings *settings);
    bool isInitialized() const {return m_initialized;}

    bool createVirtualPatient(const QString &name, const QString &secondname, const QString &firstname,
                              const QString &gender, const int title, const QDate &dob,
                              const QString &country, const QString &note,
                              const QString &street, const QString &zip, const QString &city,
                              QString uuid, const int lkid,
                              const QString &photoFile = QString(), const QDate &death = QDate());

    QString patientUuid(const QString &birthname, const QString &secondname, const QString &firstname,
                        const QString &gender, const QDate &dob) const;
    bool isPatientExists(const QString &birthname, const QString &secondname, const QString &firstname,
                         const QString &gender, const QDate &dob) const;

//    bool mergePatients(const QString &uuid1, const QString &uuid2);

    void toTreeWidget(QTreeWidget *tree);

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    bool m_initialized;
    static PatientBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace Patients

#endif // PATIENTBASE_H
