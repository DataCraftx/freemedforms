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
#ifndef IUSER_H
#define IUSER_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QVariant>

/**
 * \file iuser.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 13 June 2010
*/

namespace Core {

/**
  \brief Use this class to avoid any plugin dependencies (other than Core), when needing to access to patients datas.
*/

class CORE_EXPORT IUser : public QObject
{
    Q_OBJECT
public:
    enum DataRepresentation {
        // ORDER SHOULD NEVER CHANGE
        Id = 0,
        Uuid,           /*!< \brief Unique identifier the each users */
        Validity,       /*!< \brief Validity of this user */
        IsVirtual,      /*!< \brief User is virtual ?*/
        Login64,        /*!< \brief Crypted login \sa UserGlobal::loginForSQL() */
        Password,       /*!< \brief Crypted password \sa UserGlobal::crypt() */
        LastLogin,      /*!< \brief Date of the last correct login */
        Name,           /*!< \brief Name */
        SecondName,     /*!< \brief Second name */
        Firstname,      /*!< \brief Firstname */
        Mail,           /*!< \brief Mail address */
        LanguageISO,    /*!< \brief Language for interface */
        Locker,         /*!< \brief Locker */
        LocaleLanguage, /*!< \brief Language index from Core::Translators::availableLanguages() */  //12

        // ORDER CAN CHANGE
        ClearLogin,
        LocaleCodedLanguage,
        PersonalLinkId,
        FullName,
        GenderIndex, TitleIndex, Gender, Title,
        Adress, Zipcode, City, Country, IsoCountry,
        Tel1, Tel2, Tel3, Fax, PractitionerId,
        Specialities, Qualifications, Preferences,  // 31

        GenericHeader,
        GenericFooter,
        GenericWatermark,
        GenericHeaderPresence,
        GenericFooterPresence,
        GenericWatermarkPresence,
        GenericWatermarkAlignement,

        AdministrativeHeader,
        AdministrativeFooter,
        AdministrativeWatermark,
        AdministrativeHeaderPresence,
        AdministrativeFooterPresence,
        AdministrativeWatermarkPresence,
        AdministrativeWatermarkAlignement,

        PrescriptionHeader,
        PrescriptionFooter,
        PrescriptionWatermark,
        PrescriptionHeaderPresence,
        PrescriptionFooterPresence,
        PrescriptionWatermarkPresence,
        PrescriptionWatermarkAlignement,

        ManagerRights,
        DrugsRights,
        MedicalRights,
        ParamedicalRights,
        AdministrativeRights,
        AgendaRights,

        DecryptedLogin,           /*!< \brief Clear human readable login. */
        LoginHistory,             /*!< \brief Full login history of the user. */
        IsModified,
        Warn,                     /*!< \brief For debugging purpose. */
        WarnText,                 /*!< \brief For debugging purpose. */
        NumberOfColumns
    };

    enum UserRight
    {
        NoRights       = 0x00000000,
        ReadOwn        = 0x00000001,
        ReadDelegates  = 0x00000002,
        ReadAll        = 0x00000004,
        WriteOwn       = 0x00000010,
        WriteDelegates = 0x00000020,
        WriteAll       = 0x00000040,
        Print          = 0x00000100,
        Create         = 0x00000200,
        Delete         = 0x00000400,
        AllRights      = ReadOwn | ReadDelegates | ReadAll | WriteOwn | WriteDelegates | WriteAll | Print | Create | Delete
    };
    Q_DECLARE_FLAGS(UserRights, UserRight)

    IUser(QObject *parent) : QObject(parent) {}
    virtual ~IUser() {}

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;
    virtual bool hasCurrentUser() const = 0;

    virtual QVariant value(const int ref) const = 0;
    virtual bool setValue(const int ref, const QVariant &value) = 0;

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    virtual bool saveChanges() = 0;

    virtual QString fullNameOfUser(const QVariant &uid) = 0;

    void replaceTokens(QString &stringWillBeModified);

    // Some code easiers
    QString uuid() const {return value(Uuid).toString();}

Q_SIGNALS:
    void userChanged();
    void userDataChanged(const int ref);
};

}  // End Core
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::IUser::UserRights)


#endif // IUSER_H
