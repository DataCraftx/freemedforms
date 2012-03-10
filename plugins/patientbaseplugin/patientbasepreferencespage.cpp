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
#include "patientbasepreferencespage.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "patientselector.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include "ui_patientbasepreferencespage.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QDir>
#include <QByteArray>
#include <QBuffer>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

PatientBasePreferencesPage::PatientBasePreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("PatientBasePreferencesPage"); }

PatientBasePreferencesPage::~PatientBasePreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PatientBasePreferencesPage::id() const { return objectName(); }
QString PatientBasePreferencesPage::name() const { return tkTr(Trans::Constants::PATIENTS); }
QString PatientBasePreferencesPage::category() const { return tkTr(Trans::Constants::PATIENT_DATABASE); }
QString PatientBasePreferencesPage::title() const {return tr("Patient database preferences");}

void PatientBasePreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void PatientBasePreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PatientBasePreferencesPage::finish() { delete m_Widget; }

void PatientBasePreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_SELECTOR_FIELDSTOSHOW, PatientSelector::Default);
    defaultvalues.insert(Constants::S_SELECTOR_USEGENDERCOLORS, true);
    defaultvalues.insert(Constants::S_PATIENTBARCOLOR, Qt::white);
    defaultvalues.insert(Constants::S_PATIENTCHANGEONCREATION, true);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

QWidget *PatientBasePreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PatientBasePreferencesWidget(parent);
    return m_Widget;
}



PatientBasePreferencesWidget::PatientBasePreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    ui = new Ui::PatientBasePreferencesWidget;
    setObjectName("PatientBasePreferencesWidget");
    ui->setupUi(this);
    setDatasToUi();
}

PatientBasePreferencesWidget::~PatientBasePreferencesWidget()
{
    delete ui;
}

void PatientBasePreferencesWidget::setDatasToUi()
{
    ui->selectNewlyCreatedBox->setChecked(settings()->value(Constants::S_PATIENTCHANGEONCREATION).toBool());
    ui->genderColor->setChecked(settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool());
    ui->patientBarColor->setColor(QColor(settings()->value(Constants::S_PATIENTBARCOLOR).toString()));
}

void PatientBasePreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Constants::S_PATIENTCHANGEONCREATION, ui->selectNewlyCreatedBox->isChecked());
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, ui->genderColor->isChecked());
    s->setValue(Constants::S_PATIENTBARCOLOR, ui->patientBarColor->color());
}

void PatientBasePreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    LOG_FOR("PatientBasePreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("Patient preferences"));
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, true);
    s->setValue(Constants::S_PATIENTBARCOLOR, Qt::white);
    s->setValue(Constants::S_PATIENTCHANGEONCREATION, true);
    s->sync();
}

void PatientBasePreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
