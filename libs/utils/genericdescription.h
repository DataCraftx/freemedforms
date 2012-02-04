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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DATAPACK_GENERICDESCRIPTION_H
#define DATAPACK_GENERICDESCRIPTION_H

#include <utils/global_exporter.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/multilingualclasstemplate.h>

#include <QVariant>
#include <QHash>
#include <QList>
#include <QString>
QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file genericdescsription.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Jan 2012
*/

namespace Utils {
class GenericUpdateInformation;
class VersionNumber;
}

namespace Utils {

class UTILS_EXPORT GenericDescription
{
public:
    enum DataRepresentation {
        // Non translatable data
        Uuid= 0,
        Version,
        Author,
        Country,
        URL,
        AbsFileName,
        Vendor,
        Validity,
        FreeMedFormsCompatVersion,
        FreeDiamsCompatVersion,
        FreeAccountCompatVersion,
        CreationDate,
        LastModificationDate,
        GeneralIcon,

        // Translatable data
        Label,
        Category,
        ToolTip,
        Specialties,
        TypeName,
        ShortDescription,
        HtmlDescription,
        HtmlSynthesis,
        GlobalLicense,
        WebLink,
        MaxParam
    };
    enum ExtraDataReference {
        NonTranslatableExtraData = 20000,
        TranslatableExtraData = 40000
    };

    GenericDescription(const QString &rootTag = QString::null);
    virtual ~GenericDescription();

    void setRootTag(const QString &rootTag);
    void setSourceFileName(const QString &absFileName);
    QString sourceFileName() const;

    virtual void clear();

    virtual QVariant data(const int ref, const QString &lang = QString::null) const;
    virtual bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);

    virtual bool fromXmlContent(const QString &xmlContent);
    virtual bool fromXmlFile(const QString &absFileName);
    virtual bool fromDomElement(const QDomElement &root);
    virtual QString toXml() const;
    virtual bool toDomElement(QDomElement *root, QDomDocument *doc) const;

    // Manage update informations
    void addUpdateInformation(const Utils::GenericUpdateInformation &updateInfo);
    void removeUpdateInformation(int index);
    void insertUpdateInformation(int index, const Utils::GenericUpdateInformation &updateInfo);
    QList<Utils::GenericUpdateInformation> updateInformation() const;
    QList<Utils::GenericUpdateInformation> updateInformationForVersion(const QString &version) const;
    QList<Utils::GenericUpdateInformation> updateInformationForVersion(const Utils::VersionNumber &version) const;

    QString htmlUpdateInformationForVersion(const QString &version) const;
    QString htmlUpdateInformationForVersion(const Utils::VersionNumber &version) const;

    // Manage extra data
    void addNonTranslatableExtraData(const int ref, const QString &tagName);
    void addTranslatableExtraData(const int ref, const QString &tagName);

//    // Manage unknown tags (not in nontransatable, not in translatable)
//    virtual void readUnknownElement(const QDomElement &element);

    virtual void toTreeWidget(QTreeWidget *tree) const;

private:
    QHash<int, QString> nonTranslatableTagsDataReference() const;
    QHash<int, QString> translatableTagsDataReference() const;

private:
    QList<Utils::GenericUpdateInformation> m_UpdateInfos;
    QString m_RootTag, m_SourceFileName;
    QHash<int, QString> m_TranslatableExtra, m_NonTranslatableExtra;
    QHash<QString, QHash<int, QVariant> > m_Data;
};

}  // End namespace DataPack


#endif // DATAPACK_GENERICDESCRIPTION_H
