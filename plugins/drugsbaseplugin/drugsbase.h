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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGSBASE_H
#define DRUGSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

#include <utils/database.h>

#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>

/**
 * \file drugsbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 26 Feb 2011
*/

namespace MedicalUtils {
class EbmData;
}


namespace DrugsDB {
class DatabaseInfos;
class IDrug;

namespace Internal {
class DrugInfo;
class DrugsBasePrivate;

class DRUGSBASE_EXPORT DrugsBase : public QObject, public Utils::Database
{
    Q_OBJECT
    DrugsBase(QObject *parent = 0);
    bool init();

    friend class DrugsModel;
    friend class DrugsBasePrivate;
    friend class DrugInfo;

public:
    static DrugsBase *instance();
    ~DrugsBase();

    // Initializer / Checkers
    static bool isInitialized() { return m_initialized; }
    void logChronos(bool state);
    const DatabaseInfos *actualDatabaseInformations() const;
    bool isDatabaseTheDefaultOne() const;
    bool isRoutesAvailable() const;

    QVector<DatabaseInfos *> getAllDrugSourceInformations();
    DatabaseInfos *getDrugSourceInformations(const QString &drugSourceUid);

    bool refreshAllDatabases();
    bool refreshDrugsBase();
    bool changeCurrentDrugSourceUid(const QVariant &uid);
    bool refreshDosageBase();

    // Manage drugs
    QString getDrugName(const QString &uid1, const QString &uid2 = QString::null, const QString &uid3 = QString::null) const;
    IDrug *getDrugByUID(const QVariant &uid1, const QVariant &uid2 = QVariant(), const QVariant &uid3 = QVariant(), const QVariant &oldUid = QVariant(), const QString &sourceUid = QString::null);
    IDrug *getDrugByOldUid(const QVariant &oldUid, const QString &sourceUid = QString::null);
    QVariantList getDrugUids(const QVariant &drugId);
    IDrug *getDrugByDrugId(const QVariant &drugId);

    // Manage labels
    QString getLabel(const int masterLid, const QString &lang = QString::null);

    // Used by the GlobalDrugsModel
    QStringList getDrugCompositionAtcCodes(const QVariant &drugId);
    QStringList getDrugInns(const QVariant &drugId);
    QStringList getRouteLabels(const QVariant &drugId, const QString &lang = QString::null);
    QStringList getFormLabels(const QVariant &drugId, const QString &lang = QString::null);
    QHash<QString, QString> getDrugFullComposition(const QVariant &drugId, const QString &lang = QString::null);
    QStringList getDrugMolecularComposition(const QVariant &drugId, const QString &lang = QString::null);

    // Manage Dosages
    void checkDosageDatabaseVersion();
    static QString dosageCreateTableSqlQuery();
    QHash<QString, QString> getDosageToTransmit();
    bool markAllDosageTransmitted(const QStringList &dosageUuids);
    QList<QVariant> getAllUIDThatHaveRecordedDosages() const;
    QMultiHash<int,QString> getAllINNThatHaveRecordedDosages() const;

    // Manage drug classification
    int getAtcCodeForMoleculeId(const int molId) const;
    QString getAtcLabel(const int atcId) const;
    QString getAtcLabel(const QString &code) const;
    QString getAtcCode(const int atcId) const;
    QString getInnDenominationFromSubstanceCode(const int molId) const;

//    QStringList getIamClassDenomination(const int molId);
//    QVector<int> getAllInnAndIamClassesIndex(const int molId);

    QVector<int> getLinkedMoleculeCodes(QVector<int> &atc_ids) const;
    QVector<int> getLinkedMoleculeCodes(const int atc_id) const;
    QVector<int> getLinkedMoleculeCodes(const QString &atcLabel) const;
    QVector<int> getLinkedAtcIds(const QVector<int> &mids) const;
    QVector<int> getLinkedAtcIds(const int mid) const;

    QVector<int> getAllMoleculeCodeWithAtcStartingWith(const QString &code) const;

    QVector<MedicalUtils::EbmData *> getAllSourcesFromTree(const QList<int> &allInnAndInteractingClassesIds);


//protected:
public:
    static const QString separator;

    // managins drugs
//    bool drugsINNIsKnown(const IDrug *drug);

Q_SIGNALS:
    void drugsBaseIsAboutToChange();
    void drugsBaseHasChanged();
    void dosageBaseIsAboutToChange();
    void dosageBaseHasChanged();


private:
    bool createDatabase(const QString & connectionName , const QString & dbName,
                        const QString & pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString & /*login*/, const QString & /*pass*/,
                        const int /*port*/,
                        CreationOption /*createOption*/
                       );
private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    // intialization state
    static DrugsBase *m_Instance;
    static bool m_initialized;
    DrugsBasePrivate *d;
    bool m_IsDefaultDB;
};

}  // End Internal
}  // End DrugsDB

#endif
