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
#include "drugscentralwidget.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/interactionmanager.h>
#include <drugsbaseplugin/globaldrugsmodel.h>
#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/druginteractioninformationquery.h>
#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/idruginteractionalert.h>

#include <drugsplugin/drugswidget/drugselector.h>
#include <drugsplugin/drugswidget/prescriptionviewer.h>
#include <drugsplugin/drugswidget/dynamicalert.h>
#include <drugsplugin/dosagedialog/mfDosageCreatorDialog.h>
#include <drugsplugin/dosagedialog/mfDosageDialog.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/iuser.h>

#include <templatesplugin/templatescreationdialog.h>

#include "ui_drugscentralwidget.h"
#include <QTreeWidget>

using namespace DrugsWidget;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::UniqueIDManager *uid() {return Core::ICore::instance()->uniqueIDManager();}
static inline Core::ContextManager *contextManager() {return Core::ICore::instance()->contextManager();}
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

/** \brief Constructor */
DrugsCentralWidget::DrugsCentralWidget(QWidget *parent) :
    QWidget(parent), m_CurrentDrugModel(0), m_ui(0)
{
    // create instance of DrugsManager
    DrugsWidgetManager::instance();
}

/** \brief Initialize the widget after the ui was setted */
bool DrugsCentralWidget::initialize(bool hideSelector)
{
    m_ui = new DrugsWidget::Internal::Ui::DrugsCentralWidget();
    m_ui->setupUi(this);

    // create context
    m_Context = new Internal::DrugsContext(this);
    m_Context->setContext(QList<int>() << uid()->uniqueIdentifier(Constants::C_DRUGS_PLUGINS));
    contextManager()->addContextObject(m_Context);

    // create model view for selected drugs list
    m_CurrentDrugModel = new DrugsDB::DrugsModel(this);
    m_ui->m_PrescriptionView->initialize();
    m_ui->m_PrescriptionView->setModel(m_CurrentDrugModel);

    m_ui->m_DrugSelector->initialize();
    m_ui->m_DrugSelector->setFocus();
    if (hideSelector)
        m_ui->m_DrugSelector->hide();

    DrugsWidgetManager::instance()->setCurrentView(this);

    changeFontTo(QFont(settings()->value(Constants::S_VIEWFONT).toString(), settings()->value(Constants::S_VIEWFONTSIZE).toInt()));

    return true;
}

QListView *DrugsCentralWidget::prescriptionListView()
{
    if (!m_ui)
        return 0;
    return m_ui->m_PrescriptionView->listview();
}

PrescriptionViewer *DrugsCentralWidget::prescriptionView()
{
    if (!m_ui)
        return 0;
    return m_ui->m_PrescriptionView;
}

Internal::DrugSelector *DrugsCentralWidget::drugSelector()
{
    if (!m_ui)
        return 0;
    return m_ui->m_DrugSelector;
}

DrugsDB::DrugsModel *DrugsCentralWidget::currentDrugsModel() const
{
    return m_CurrentDrugModel;
}

void DrugsCentralWidget::setCurrentSearchMethod(int method)
{
    if (!m_ui)
        return;
    m_ui->m_DrugSelector->setSearchMethod(method);
}

void DrugsCentralWidget::createConnections()
{
    connect(m_ui->m_DrugSelector, SIGNAL(drugSelected(QVariant)),
            this, SLOT(onSelectorDrugSelected(QVariant)));
    connect(prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
            m_ui->m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)));
}

void DrugsCentralWidget::disconnect()
{
    prescriptionListView()->disconnect( prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
             m_ui->m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)));
    m_ui->m_DrugSelector->disconnect(m_ui->m_DrugSelector, SIGNAL(drugSelected(QVariant)),
                                     this, SLOT(onSelectorDrugSelected(QVariant)));
}

void DrugsCentralWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_ui->m_DrugSelector->setFocus();
}

/**
  \brief Slot called when is selected from the drugSelector.
  Verify that the drug isn't already prescribed (if it is warn user and stop). \n
  Add the drug to the DrugsModel and open the DosageCreatorDialog\n
*/
void DrugsCentralWidget::onSelectorDrugSelected(const QVariant &drugId)
{
    // If drug already in prescription --> Stop
    if (m_CurrentDrugModel->containsDrug(drugId)) {
        Utils::warningMessageBox(tr("Can not add this drug to your prescription."),
                                 tr("Prescription can not contains twice the sample pharmaceutical drug.\n"
                                    "Drug %1 is already in your prescription")
                                 .arg(m_CurrentDrugModel->drugData(drugId, DrugsDB::Constants::Drug::Denomination).toString()),
                                 tr("If you want to change the dosage of this drug please double-click on it in the prescription box."));
        return;
    }

    // Add drug to the model
    m_CurrentDrugModel->addDrug(drugId);

    // Start dynamicAlert (ProcessTime == BeforePrescription)
    DrugsDB::DrugInteractionInformationQuery query;
    query.processTime = DrugsDB::DrugInteractionInformationQuery::BeforePrescription;
    query.result = m_CurrentDrugModel->drugInteractionResult();
    query.relatedDrug = m_CurrentDrugModel->getDrug(drugId);
    DynamicAlert::DialogResult result = DynamicAlert::executeDynamicAlert(query, this);
    if (result==DynamicAlert::DynamicAlertAccepted) {
        m_CurrentDrugModel->removeLastInsertedDrug();
        return;
    }

    // show the protocol creator/selector dialog is needed
    if (!m_CurrentDrugModel->isSelectionOnlyMode()) {
        Internal::DosageCreatorDialog dlg(this, m_CurrentDrugModel->dosageModel(drugId));
        if (dlg.exec()==QDialog::Rejected) {
            m_CurrentDrugModel->removeLastInsertedDrug();
        } else {
            // Start dynamicAlert (ProcessTime == AfterPrescription)
//            query.processTime = DrugsDB::DrugInteractionInformationQuery::AfterPrescription;
//            result = DynamicAlert::executeDynamicAlert(query, this);
//            if (result==DynamicAlert::DynamicAlertAccepted) {
//                m_CurrentDrugModel->removeLastInsertedDrug();
//                return;
//            }
        }
        m_ui->m_PrescriptionView->listview()->update();
    }
}

/** \brief Change the font of the viewing widget */
void DrugsCentralWidget::changeFontTo(const QFont &font)
{
    m_ui->m_DrugSelector->setFont(font);
    m_ui->m_PrescriptionView->listview()->setFont(font);
}

bool DrugsCentralWidget::printPrescription()
{
    DrugsDB::DrugsIO io;
    return io.printPrescription(m_CurrentDrugModel);
}

void DrugsCentralWidget::printPreview()
{
    DrugsDB::DrugsIO io;
    io.prescriptionPreview(m_CurrentDrugModel);
}

bool DrugsCentralWidget::createTemplate()
{
    if (m_CurrentDrugModel->rowCount() == 0)
        return false;
    // get the template content
    DrugsDB::DrugsIO io;
    QString content = io.prescriptionToXml(m_CurrentDrugModel, "");
    // create a new template with it
    Templates::TemplatesCreationDialog dlg(this);
    dlg.setTemplateContent(content);
    dlg.setTemplateSummary(io.prescriptionToHtml(m_CurrentDrugModel, "", DrugsDB::DrugsIO::SimpleVersion));
    dlg.setTemplateMimeTypes(io.prescriptionMimeTypes());
    dlg.setUserUuid(user()->uuid());
    dlg.exec();
    return true;
}

void DrugsCentralWidget::showDrugsDatabaseInformations()
{
    const DrugsDB::DatabaseInfos *info = DrugsDB::Internal::DrugsBase::instance()->actualDatabaseInformations();
    if (!info)
        return;
    QDialog dlg(this, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree2(&dlg);
    tree2.setColumnCount(2);
    tree2.header()->hide();
    info->toTreeWidget(&tree2);

    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    DrugsDB::Internal::DrugsBase::instance()->setConnectionName(DrugsDB::Constants::DB_DRUGS_NAME);
    DrugsDB::Internal::DrugsBase::instance()->toTreeWidget(&tree);

    lay.addWidget(&tree);
    lay.addWidget(&tree2);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}

void DrugsCentralWidget::showDosagesDatabaseInformations()
{
    QDialog dlg(this, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    DrugsDB::Internal::DrugsBase::instance()->setConnectionName(Dosages::Constants::DB_DOSAGES_NAME);
    DrugsDB::Internal::DrugsBase::instance()->toTreeWidget(&tree);
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}
