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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PATIENTCREATORWIZARD_H
#define PATIENTCREATORWIZARD_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QWizard>
#include <QWizardPage>

/**
 * \file patientcreatorwizard.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 25 May 2010
*/


namespace Patients {
class PatientModel;
class IdentityWidget;
class IdentityPage;

class PATIENT_EXPORT PatientCreatorWizard : public QWizard
{
    Q_OBJECT
public:
    enum Pages {
         Page_Identity
    };

    PatientCreatorWizard(QWidget *parent);

protected:
    void done(int r);

private:
    IdentityPage *m_Page;
    bool m_Saved;
};

class IdentityPage: public QWizardPage
{
    Q_OBJECT
public:
    IdentityPage(QWidget *parent = 0);
    bool validatePage();

    QString lastInsertedUuid() const {return m_uuid;}

private:
    PatientModel *m_Model;
    IdentityWidget *m_Identity;
    QString m_uuid;
};


} // End namespace Patients

#endif // PATIENTCREATORWIZARD_H
