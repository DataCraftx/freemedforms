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
#ifndef ICDDATABASE_H
#define ICDDATABASE_H

#include <icdplugin/icd_exporter.h>

#include <utils/database.h>

/**
 * \file icddatabase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 18 Nov 2010
*/


namespace ICD {
class IcdPlugin;

namespace Internal {
class IcdDatabasePrivate;
class IcdAssociation;
}

class ICD_EXPORT IcdDatabase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class ICD::IcdPlugin;
    IcdDatabase(QObject *parent = 0);

public:
    static IcdDatabase *instance();
    ~IcdDatabase();

    // Initializer / Checkers
    static bool isInitialized() { return m_initialized; }
    void logChronos(bool state);
    QString getDatabaseVersion();
    bool isDownloadAndPopulatingNeeded() const;

    QList<int> getHeadersSID(const QVariant &SID);

    QVariant getSid(const QString &code);
    QVariant getIcdCode(const QVariant &SID);
    QString getDagStarCode(const QVariant &SID);
    QString invertDagCode(const QString &dagCode) const;
    bool isDagetADag(const QString &dagCode) const;
    QString getHumanReadableIcdDaget(const QVariant &SID);
    QVariant getIcdCodeWithDagStar(const QVariant &SID);

    QVector<int> getDagStarDependencies(const QVariant &SID);
    Internal::IcdAssociation getAssociation(const QVariant &mainSID, const QVariant &associatedSID);

    bool codeCanBeUsedAlone(const QVariant &SID);

    QString getLabelFromLid(const QVariant &LID);
    QString getSystemLabel(const QVariant &SID);
    QStringList getAllLabels(const QVariant &SID, const int libelleFieldLang = -1);
    QString getAssociatedLabel(const QVariant &mainSID, const QVariant &associatedSID);
    QStringList getIncludedLabels(const QVariant &SID);

    QVector<int> getExclusions(const QVariant &SID);

    QString getMemo(const QVariant &SID);

Q_SIGNALS:
    void databaseInitialized();

protected Q_SLOTS:
    bool init();

private Q_SLOTS:
    void refreshLanguageDependCache();

private:
    // intialization state
    static IcdDatabase *m_Instance;
    static bool m_initialized;
    Internal::IcdDatabasePrivate *d;
};

} // End namespace ICD


#endif // ICDDATABASE_H
