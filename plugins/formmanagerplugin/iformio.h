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
#ifndef IFORMIO_H
#define IFORMIO_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <utils/genericdescription.h>
#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/constanttranslations.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QHash>
#include <QVariant>
#include <QList>
#include <QPixmap>

/**
 * \file iformio.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Jan 2012
*/

namespace Utils {
class VersionNumber;
class GenericUpdateInformation;
}

namespace Form {
class FormMain;

class FORM_EXPORT FormIOQuery
{
public:
    enum TypeOfForm {
        CompleteForms  = 0x001,
        SubForms       = 0x002
    };
    Q_DECLARE_FLAGS(TypesOfForm, TypeOfForm)

    FormIOQuery();
    virtual ~FormIOQuery() {}

    void setTypeOfForms(const TypesOfForm type) {m_type=type;}
    TypesOfForm typeOfForms() const {return m_type;}

    void setIsoLanguageAndCountry(const QStringList &langs) {m_langs=langs;}
    QStringList &isoLanguagesAndCountry() {return m_langs;}

    void setSpecialties(const QStringList &spe) {m_spe=spe;}
    QStringList &specialties() {return m_spe;}

    void setAuthors(const QStringList &authors) {m_authors=authors;}
    QStringList &authors() {return m_authors;}

    void setFormUuid(const QString &uuidOrAbsPath) {m_uuid=uuidOrAbsPath;}
    QString formUuid() const {return m_uuid;}

    /** Use this to force the reader to get the information (description, form content) directly from the file even if the form is already stored in the database. */
    void setForceFileReading(const bool force) {m_ForceFile = force;}
    bool forceFileReading() const {return m_ForceFile;}

    void setGetAllAvailableForms(bool state) {m_AllForms=state;}
    bool getAllAvailableForms() {return m_AllForms;}

    void setGetAllAvailableFormDescriptions(bool state) {m_AllDescr=state;}
    bool getAllAvailableFormDescriptions() const {return m_AllDescr;}

    void setGetScreenShots(bool get) {m_GetShots=get;}
    bool getScreenShots() const {return m_GetShots;}

private:
    TypesOfForm m_type;
    QStringList m_langs, m_spe, m_authors;
    QString m_uuid;
    bool m_ForceFile, m_AllForms, m_AllDescr, m_GetShots;
};

//class FORM_EXPORT FormIOResult
//{
//public:
//    FormIOResult(const FormIOQuery &query);
//    virtual ~FormIOResult();

//    QString originalFormUuid() const;

//    QList<Form::FormMain *> rootForms() const {return m_RootForms;}
//    void setRootForms(const QList<Form::FormMain *> &list) const {m_RootForms=list;}

//private:
//    FormIOQuery m_Query;
//    QList<Form::FormMain *> m_RootForms;
//};

}
Q_DECLARE_OPERATORS_FOR_FLAGS(Form::FormIOQuery::TypesOfForm)

namespace Form {
class IFormIO;
namespace Internal {
struct FormIODescriptionPrivate;
}

class FORM_EXPORT FormIODescription : public Utils::GenericDescription
{
public:
    enum ExtraDataRepresentation {
        UuidOrAbsPath = Utils::GenericDescription::MaxParam + 1,
        IsCompleteForm,
        IsSubForm,
        IsPage,
        ScreenShotsPath,
        FromDatabase,
        MaxParam
    };

    FormIODescription();
    virtual ~FormIODescription();

    void setIoFormReader(IFormIO *reader);
    IFormIO *reader() const;

    QVariant data(const int ref, const QString &lang = QString::null) const;
    bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);

    // Manage screenshots
    bool hasScreenShots() const;
    void addScreenShot(const QString &name, const QPixmap &shot);
    QList<QPixmap> screenShots() const;
    QPixmap screenShot(const QString &name) const;

    void toTreeWidget(QTreeWidget *tree) const;
    QString toHtml() const;

private:
    Internal::FormIODescriptionPrivate *d_formIO;
};

class FORM_EXPORT IFormIO : public QObject
{
    Q_OBJECT
public:
    IFormIO(QObject *parent=0) : QObject(parent) {}
    virtual ~IFormIO() {}

    virtual QString name() const = 0;

    virtual void muteUserWarnings(bool state) = 0;

    virtual bool canReadForms(const QString &uuidOrAbsPath) const = 0;

    virtual FormIODescription *readFileInformation(const QString &uuidOrAbsPath) const = 0;
    virtual QList<FormIODescription *> getFormFileDescriptions(const FormIOQuery &query) const = 0;

    virtual QList<Form::FormMain *> loadAllRootForms(const QString &uuidOrAbsPath = QString::null) const = 0;
    virtual bool loadPmhCategories(const QString &uuidOrAbsPath) const = 0; // must invalidate all existing mime before insertion of new categories

    virtual QList<QPixmap> screenShots(const QString &uuidOrAbsPath) const = 0;
    virtual QPixmap screenShot(const QString &uuidOrAbsPath, const QString &name) const = 0;

    virtual bool saveForm(QObject *treeRoot) = 0;

    virtual QString lastError() const = 0;

    virtual void checkForUpdates() const = 0;
};

} // end Form

FORM_EXPORT QDebug operator<<(QDebug dbg, const Form::FormIODescription &c);
FORM_EXPORT QDebug operator<<(QDebug dbg, const Form::FormIODescription *c);

#endif // IFORMIO_H
