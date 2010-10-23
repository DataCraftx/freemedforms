/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "mfDrugsPreferences.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/mfDrugSelector.h>

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsbase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>

#include <QPixmap>

using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  DrugsViewOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsViewOptionsPage::DrugsViewOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsViewOptionsPage"); }

DrugsViewOptionsPage::~DrugsViewOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsViewOptionsPage::id() const { return objectName(); }
QString DrugsViewOptionsPage::name() const { return tr("View"); }
QString DrugsViewOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void DrugsViewOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugsViewOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsViewOptionsPage::finish() { delete m_Widget; }

void DrugsViewOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(S_VIEWFONT, QFont());
    defaultvalues.insert(S_VIEWFONTSIZE, QFont().pointSize());
    defaultvalues.insert(S_HISTORYSIZE, 20);
    defaultvalues.insert(S_DRUGHISTORY, QVariant());
    defaultvalues.insert(S_DRUGFONT,QFont());
    defaultvalues.insert(S_PRESCRIPTIONFONT,QFont());
    defaultvalues.insert(DrugsDB::Constants::S_LEVELOFWARNING, 1);
    defaultvalues.insert(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION,true);
    defaultvalues.insert(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES,true);
    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);
    defaultvalues.insert(S_DYNAMICALERTS, true);
    defaultvalues.insert(S_DYNAMICALERTS_LEVEL, DrugsDB::Constants::Interaction::ContreIndication);
    defaultvalues.insert(S_PATIENTNAMESORDER, 0);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *DrugsViewOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsViewWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  DrugsViewOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsSelectorOptionsPage::DrugsSelectorOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsSelectorOptionsPage"); }

DrugsSelectorOptionsPage::~DrugsSelectorOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsSelectorOptionsPage::id() const { return objectName(); }
QString DrugsSelectorOptionsPage::name() const { return tr("Selector"); }
QString DrugsSelectorOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void DrugsSelectorOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugsSelectorOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsSelectorOptionsPage::finish() { delete m_Widget; }

void DrugsSelectorOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME,true);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWROUTE, false);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWFORM, false);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH, false);
    defaultvalues.insert(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES, false);
    defaultvalues.insert(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES,true);
    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);
    defaultvalues.insert(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR, "red");
    defaultvalues.insert(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR, "yellow");

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *DrugsSelectorOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsSelectorWidget(parent);
    return m_Widget;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  DrugsPrintOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsPrintOptionsPage::DrugsPrintOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsPrintOptionsPage"); }

DrugsPrintOptionsPage::~DrugsPrintOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsPrintOptionsPage::id() const { return objectName(); }
QString DrugsPrintOptionsPage::name() const { return tr("Printing"); }
QString DrugsPrintOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void DrugsPrintOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugsPrintOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsPrintOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
                         QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
    defaultvalues.insert(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
                         QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
    defaultvalues.insert(DrugsDB::Constants::S_PRINTLINEBREAKBETWEENDRUGS, true);
    defaultvalues.insert(DrugsDB::Constants::S_PRINTDUPLICATAS, true);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }

    defaultvalues.clear();

    if (Core::ICore::instance()->updatePreferences()) {
        Utils::Log::addMessage(this, tr("Updating posologic sentence."));
        defaultvalues.insert(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
                             QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
        defaultvalues.insert(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
                             QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
        foreach(const QString &k, defaultvalues.keys()) {
            settings()->setValue(k, defaultvalues.value(k));
        }
    }

    QTextDocument t;
    t.setHtml(settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML).toString());
    if (settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN).isNull() ||
        settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML).isNull() ||
        t.toPlainText().simplified().isEmpty()) {
        Utils::Log::addMessage(this, "Restoring prescription sentence to its defaults");
        settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
                             qApp->translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
        settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
                             qApp->translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
    }
    settings()->sync();
}

void DrugsPrintOptionsPage::finish() { delete m_Widget; }

QWidget *DrugsPrintOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsPrintWidget(parent);
    return m_Widget;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  DrugsUserOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsUserOptionsPage::DrugsUserOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsUserOptionsPage"); }

DrugsUserOptionsPage::~DrugsUserOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsUserOptionsPage::id() const { return objectName(); }
QString DrugsUserOptionsPage::name() const { return tr("User's documents"); }
QString DrugsUserOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void DrugsUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugsUserOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsUserOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_WATERMARKPRESENCE, Print::Printer::DuplicataOnly);
    defaultvalues.insert(DrugsDB::Constants::S_WATERMARKALIGNEMENT, Qt::AlignCenter);
    defaultvalues.insert(DrugsDB::Constants::S_WATERMARK_HTML, DrugsDB::Constants::S_DEF_WATEMARKHTML);
    defaultvalues.insert(DrugsDB::Constants::S_USERHEADER, DrugsDB::Constants::S_DEF_USERHEADER);
    defaultvalues.insert(DrugsDB::Constants::S_USERFOOTER, QVariant());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DrugsUserOptionsPage::finish() { delete m_Widget; }

QWidget *DrugsUserOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsUserWidget(parent);
    return m_Widget;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  DrugsExtraOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsExtraOptionsPage::DrugsExtraOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsExtraOptionsPage"); }

DrugsExtraOptionsPage::~DrugsExtraOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsExtraOptionsPage::id() const { return objectName(); }
QString DrugsExtraOptionsPage::name() const { return tr("Extras"); }
QString DrugsExtraOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void DrugsExtraOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugsExtraOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}
void DrugsExtraOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_HIDELABORATORY, false);
    defaultvalues.insert(DrugsDB::Constants::S_ALD_PRE_HTML, DrugsDB::Constants::S_DEF_ALD_PRE_HTML);
    defaultvalues.insert(DrugsDB::Constants::S_ALD_POST_HTML, DrugsDB::Constants::S_DEF_ALD_POST_HTML);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DrugsExtraOptionsPage::finish() { delete m_Widget; }

QWidget *DrugsExtraOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsExtraWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsViewWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsViewWidget::DrugsViewWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
}

void DrugsViewWidget::setDatasToUi()
{
    Core::ISettings *s = settings();
    //    fontSizeSpin->setValue(m_fontSize);
    QString userName = s->value(S_USER).toString();
    if ((!userName.isEmpty()) && (userName.startsWith("test_"))) {
        testerBox->setChecked(true);
        userNameEdit->setText(userName);
        userPasswordEdit->setText(s->value(S_PASSWORD).toString());
        userPasswordEdit->setEnabled(false);
    }
    else
        testerBox->setChecked(false);

    viewFontSizeSpin->setValue(s->value(S_VIEWFONTSIZE, 12).toInt());
    historicSizeSpin->setValue(s->value(S_HISTORYSIZE).toInt());
    levelOfWarningCombo->setCurrentIndex(s->value(DrugsDB::Constants::S_LEVELOFWARNING).toInt());

    viewFontCombo->setCurrentFont(s->value(S_VIEWFONT).toString());
    viewFontSizeSpin->setValue(s->value(S_VIEWFONTSIZE).toInt());
    patientNameOrder->setCurrentIndex(s->value(S_PATIENTNAMESORDER, 0).toInt());
    showIconsCheck->setChecked(s->value(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION).toBool());

    dynamicAlerts->setChecked(s->value(S_DYNAMICALERTS).toBool());
    int level = s->value(S_DYNAMICALERTS_LEVEL).toInt();
    switch (level)
    {
    case DrugsDB::Constants::Interaction::ContreIndication: dynamicAlertsLevel->setCurrentIndex(2); break;
    case DrugsDB::Constants::Interaction::Deconseille: dynamicAlertsLevel->setCurrentIndex(1); break;
    case 0: dynamicAlertsLevel->setCurrentIndex(0); break;
    }
}

void DrugsViewWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    // manage font size
    s->setValue(S_VIEWFONTSIZE, viewFontSizeSpin->value());
    // set testers datas
    if ((userNameEdit->text().startsWith("test_")))
        s->setValue(S_USER, userNameEdit->text());

    if (!userPasswordEdit->text().isEmpty())
        s->setValue(S_PASSWORD, userPasswordEdit->text());

    // manage history size
    s->setValue(S_HISTORYSIZE, historicSizeSpin->value());
    s->setValue(S_DRUGHISTORY, QVariant());
    s->setValue(DrugsDB::Constants::S_LEVELOFWARNING , levelOfWarningCombo->currentIndex());

    s->setValue(S_VIEWFONT , viewFontCombo->currentFont());
    s->setValue(S_VIEWFONTSIZE, viewFontSizeSpin->value());
    QFont font = viewFontCombo->currentFont();
    font.setPointSize(viewFontSizeSpin->value());
    if (DrugsWidget::DrugsWidgetManager::instance()->currentView())
        DrugsWidget::DrugsWidgetManager::instance()->currentView()->changeFontTo(font);

    s->setValue(S_PATIENTNAMESORDER, patientNameOrder->currentIndex());
    s->setValue(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION, showIconsCheck->isChecked());

    s->setValue(S_DYNAMICALERTS, dynamicAlerts->isChecked());
    switch (dynamicAlertsLevel->currentIndex())
    {
    case 2: s->setValue(S_DYNAMICALERTS_LEVEL, DrugsDB::Constants::Interaction::ContreIndication); break;
    case 1: s->setValue(S_DYNAMICALERTS_LEVEL, DrugsDB::Constants::Interaction::Deconseille); break;
    case 0: s->setValue(S_DYNAMICALERTS_LEVEL, 0); break;
    }
}

void DrugsViewWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsViewWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(S_VIEWFONT , QFont());
    s->setValue(S_VIEWFONTSIZE, QFont().pointSize());
    s->setValue(S_HISTORYSIZE, 20);
    s->setValue(S_DRUGHISTORY, QVariant());
    s->setValue(DrugsDB::Constants::S_LEVELOFWARNING , 0);
    s->setValue(DrugsDB::Constants::S_SHOWICONSINPRESCRIPTION , true);
    s->setValue(S_DYNAMICALERTS, true);
    s->setValue(S_DYNAMICALERTS_LEVEL, DrugsDB::Constants::Interaction::ContreIndication);
    s->setValue(S_PATIENTNAMESORDER, 0);

    s->setValue(S_DRUGFONT , QFont().toString());
    s->setValue(S_PRESCRIPTIONFONT , QFont().toString());

    s->sync();
}

void DrugsViewWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  DrugsSelectorWidget  ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsSelectorWidget::DrugsSelectorWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
}

void DrugsSelectorWidget::setDatasToUi()
{
    drugsBrandNameBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME).toBool());
    routeBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWROUTE).toBool());
    formBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWFORM).toBool());
    compoBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES).toBool());
    strengthBox->setChecked(settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH).toBool());

    useBackgroundForDosages->setChecked(settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool());
    backgroundDosagesAvailableButton->setColor(QColor(settings()->value(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString()));
    backgroundAllergiesButton->setColor(QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString()));
    backgroundIntoleranceButton->setColor(QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString()));
}

void DrugsSelectorWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME, drugsBrandNameBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWROUTE, routeBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWFORM, formBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES, compoBox->isChecked());
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH, strengthBox->isChecked());

    s->setValue(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES, useBackgroundForDosages->isChecked());
    s->setValue(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, backgroundDosagesAvailableButton->color());
    s->setValue(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR, backgroundAllergiesButton->color());
    s->setValue(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR, backgroundIntoleranceButton->color());

    s->sync();
    if (DrugsWidget::DrugsWidgetManager::instance()->currentView())
        DrugsWidget::DrugsWidgetManager::instance()->currentView()->drugSelector()->updateDrugsViewColumns();
}

void DrugsSelectorWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsSelectorWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsSelectorWidget"));

    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWDRUGSNAME, true);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWROUTE, false);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWFORM, false);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWMOLECULES, false);
    s->setValue(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH, false);

    s->setValue(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES,true);
    s->setValue(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);
    s->setValue(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR, "red");
    s->setValue(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR, "yellow");

    s->sync();

//    DrugsWidget::DrugsWidgetManager::instance()->currentView()->drugSelector()->updateDrugsViewColumns();
}

void DrugsSelectorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  DrugsPrintWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsPrintWidget::DrugsPrintWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    oldGroupBox->hide();

    // Create a virtual drug and prescription
    using namespace DrugsDB::Constants;
    /** \todo Change the UID */
    drug = DrugsDB::Internal::DrugsBase::instance()->getDrugByUID("-1");
    if (!drug) {
        Utils::Log::addError(this, "Unable to retreive a drug from the database", __FILE__, __LINE__);
    } else {
        drug->setPrescriptionValue(Prescription::IntakesFrom, 1);
        drug->setPrescriptionValue(Prescription::IntakesTo, 3);
        drug->setPrescriptionValue(Prescription::IntakesScheme, tkTr(Trans::Constants::INTAKES));
        drug->setPrescriptionValue(Prescription::IntakesUsesFromTo, true);
        drug->setPrescriptionValue(Prescription::Period, 2);
        drug->setPrescriptionValue(Prescription::PeriodScheme, tkTr(Trans::Constants::DAY_S));
        drug->setPrescriptionValue(Prescription::IntakesIntervalOfTime, 2);
        drug->setPrescriptionValue(Prescription::IntakesIntervalScheme, tkTr(Trans::Constants::DAY_S));
        drug->setPrescriptionValue(Prescription::DurationFrom, 1);
        drug->setPrescriptionValue(Prescription::DurationTo, 3);
        drug->setPrescriptionValue(Prescription::DurationScheme, tkTr(Trans::Constants::WEEK_S));
        drug->setPrescriptionValue(Prescription::DurationUsesFromTo, true);
        drug->setPrescriptionValue(Prescription::MealTimeSchemeIndex, 1);
        drug->setPrescriptionValue(Prescription::Note, tr("This a note to take into account<br />written in two lines..."));
        QString daily = "<" + Trans::ConstantTranslations::dailySchemeXmlTagList().at(1) + "=1>";
        daily += "<" + Trans::ConstantTranslations::dailySchemeXmlTagList().at(3) + "=1>";
        daily += "<" + Trans::ConstantTranslations::dailySchemeXmlTagList().at(6) + "=1>";
        drug->setPrescriptionValue(Prescription::DailyScheme, daily);

        setDatasToUi();

        connect(defaultFormattingButton, SIGNAL(clicked()), this, SLOT(resetToDefaultFormatting()));
        connect(prescriptionFormatting->textEdit(), SIGNAL(textChanged()), this, SLOT(updateFormatting()));
    }
    // formatingSample
}

void DrugsPrintWidget::setDatasToUi()
{
    QFont drugsFont;
    drugsFont.fromString(settings()->value(S_DRUGFONT).toString());
    QFont prescrFont;
    prescrFont.fromString(settings()->value(S_PRESCRIPTIONFONT).toString());

    prescriptionFormatting->textEdit()->setHtml(settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML).toString());
    updateFormatting();
    lineBreakCheck->setChecked(settings()->value(DrugsDB::Constants::S_PRINTLINEBREAKBETWEENDRUGS).toBool());
    printDuplicataCheck->setChecked(settings()->value(DrugsDB::Constants::S_PRINTDUPLICATAS).toBool());
}

void DrugsPrintWidget::resetToDefaultFormatting()
{
    prescriptionFormatting->setHtml(qApp->translate("mfDrugsConstants", DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
}

static inline QString getFullPrescription(DrugsDB::Internal::DrugsData *drug, bool toHtml, const QString &tmp)
{ return DrugsDB::DrugsModel::getFullPrescription(drug,toHtml,tmp); }

void DrugsPrintWidget::updateFormatting()
{
    QString tmp = prescriptionFormatting->textEdit()->toHtml();
    formatingSample->setHtml(getFullPrescription(drug, true, tmp));
}

void DrugsPrintWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    QString tmp = prescriptionFormatting->textEdit()->toHtml();
    tmp = Utils::toHtmlAccent(tmp);
    int cutBegin = tmp.indexOf("<p ");
    int cutEnd = tmp.indexOf("</body>");
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML, tmp.mid(cutBegin, cutEnd-cutBegin));
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN, prescriptionFormatting->textEdit()->toPlainText());
    s->setValue(DrugsDB::Constants::S_PRINTLINEBREAKBETWEENDRUGS, lineBreakCheck->isChecked());
    s->setValue(DrugsDB::Constants::S_PRINTDUPLICATAS, printDuplicataCheck->isChecked());
    s->sync();
}

void DrugsPrintWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsPrintWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsPrintWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
                qApp->translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
                qApp->translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
    s->setValue(DrugsDB::Constants::S_PRINTLINEBREAKBETWEENDRUGS, true);
    s->setValue(DrugsDB::Constants::S_PRINTDUPLICATAS, true);
    s->sync();
}

void DrugsPrintWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsUserWidget::DrugsUserWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    previewer = Print::Printer::previewer(this);
    userLayout->addWidget(previewer, 0,0);
    setDatasToUi();
}

void DrugsUserWidget::setDatasToUi()
{
    Core::ISettings *s = settings();
    previewer->setHeaderHtml(s->value(DrugsDB::Constants::S_USERHEADER).toString());
    previewer->setFooterHtml(s->value(DrugsDB::Constants::S_USERFOOTER).toString());
    previewer->setWatermarkHtml(s->value(DrugsDB::Constants::S_WATERMARK_HTML).toString());
    previewer->setWatermarkPresence(s->value(DrugsDB::Constants::S_WATERMARKPRESENCE).toInt());
}

void DrugsUserWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(DrugsDB::Constants::S_USERHEADER, previewer->headerToHtml());
    s->setValue(DrugsDB::Constants::S_USERFOOTER, previewer->footerToHtml());
    s->setValue(DrugsDB::Constants::S_WATERMARKPRESENCE, previewer->watermarkPresence());
    s->setValue(DrugsDB::Constants::S_WATERMARK_HTML, previewer->watermarkToHtml());
}

void DrugsUserWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(DrugsDB::Constants::S_WATERMARKPRESENCE, Print::Printer::DuplicataOnly);
    s->setValue(DrugsDB::Constants::S_WATERMARKALIGNEMENT, Qt::AlignCenter);
    s->setValue(DrugsDB::Constants::S_WATERMARK_HTML, DrugsDB::Constants::S_DEF_WATEMARKHTML);
    s->setValue(DrugsDB::Constants::S_USERHEADER, DrugsDB::Constants::S_DEF_USERHEADER);
    s->setValue(DrugsDB::Constants::S_USERFOOTER, "");
    s->sync();
}

void DrugsUserWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsExtraWidget::DrugsExtraWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    ALDBefore->setTypes(Editor::TextEditor::Full);
    ALDAfter->setTypes(Editor::TextEditor::Full);
    setDatasToUi();
}

void DrugsExtraWidget::setDatasToUi()
{
    Core::ISettings *s = settings();
    hideLabCheck->setChecked(s->value(DrugsDB::Constants::S_HIDELABORATORY).toBool());
    ALDBefore->textEdit()->setHtml(s->value(DrugsDB::Constants::S_ALD_PRE_HTML).toString());
    ALDAfter->textEdit()->setHtml(s->value(DrugsDB::Constants::S_ALD_POST_HTML).toString());
}

void DrugsExtraWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;
    s->setValue(DrugsDB::Constants::S_HIDELABORATORY, hideLabCheck->isChecked());

    s->setValue(DrugsDB::Constants::S_ALD_PRE_HTML, ALDBefore->textEdit()->toHtml());
    s->setValue(DrugsDB::Constants::S_ALD_POST_HTML, ALDAfter->textEdit()->toHtml());
}

void DrugsExtraWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsExtraWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(DrugsDB::Constants::S_HIDELABORATORY, false);

    s->setValue(DrugsDB::Constants::S_ALD_PRE_HTML, DrugsDB::Constants::S_DEF_ALD_PRE_HTML);
    s->setValue(DrugsDB::Constants::S_ALD_POST_HTML, DrugsDB::Constants::S_DEF_ALD_POST_HTML);
    s->sync();
}

void DrugsExtraWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
