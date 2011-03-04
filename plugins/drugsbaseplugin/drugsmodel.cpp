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
/**
  \class DrugsModel
  \brief Model asks DrugsBase to check interaction only while passing new *Drugs via addDrugs()
  or while passing QDrugsList via setDrugsList().
  activeModel() , setActiveModel()
  \todo write code documentation
*/

#include "drugsmodel.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/interactionmanager.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/dailyschememodel.h>
#include <drugsbaseplugin/globaldrugsmodel.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/serializer.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <templatesplugin/constants.h>
#include <templatesplugin/itemplates.h>
#include <templatesplugin/templatesmodel.h>

#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QHash>
#include <QPointer>
#include <QStringList>
#include <QMimeData>
#include <QDomDocument>
#include <QDir>


namespace {
    const char * const ALD_BACKGROUND_COLOR               = "khaki";
    const char * const FORTEST_BACKGROUND_COLOR           = "#EFEFEF";
    const char * const FORTEST_FOREROUND_COLOR            = "#555555";
}

using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline DrugsDB::InteractionManager *interactionManager() {return DrugsDB::InteractionManager::instance();}

DrugsDB::DrugsModel *DrugsDB::DrugsModel::m_ActiveModel = 0;

namespace DrugsDB {
namespace Internal {

class DrugsModelPrivate
{
public:
    DrugsModelPrivate() :
            m_LastDrugRequiered(0), m_ShowTestingDrugs(true),
            m_SelectionOnlyMode(false), m_IsDirty(false),
            m_InteractionResult(0)
    {
    }

    ~DrugsModelPrivate()
    {
        if (m_InteractionResult)
            delete m_InteractionResult;
        m_InteractionResult = 0;
        qDeleteAll(m_DosageModelList);
        m_DosageModelList.clear();
        qDeleteAll(m_DrugsList);
        m_DrugsList.clear();
        qDeleteAll(m_TestingDrugsList);
        m_TestingDrugsList.clear();
    }

    /** \brief Return the pointer to the drug if it is already in the drugs list, otherwise return 0 */
    IDrug *getDrug(const QVariant &drugId)
    {
        if (m_LastDrugRequiered) {
            if (m_LastDrugRequiered->drugId() == drugId) {
                return m_LastDrugRequiered;
            }
        }
        m_LastDrugRequiered = 0;
        foreach(IDrug *drug, m_DrugsList) {
            if (drug->drugId()==drugId)
                m_LastDrugRequiered = drug;
        }
        return m_LastDrugRequiered;
    }

    /**
       \brief Set drugs' data directly into the private drugsList
       \sa DrugsModel::setData()
    */
    bool setDrugData(IDrug *drug, const int column, const QVariant &value)
    {
        Q_ASSERT(drug);
        if (column == Constants::Drug::Denomination) {
            TextualDrugsData *td = static_cast<TextualDrugsData*>(drug);
            if (td) {
                td->setDenomination(value.toString());
                m_IsDirty = true;
                return true;
            } else {
                return false;
            }
        }
        if ((column < Constants::Prescription::Id) || (column > Constants::Prescription::MaxParam))
            return false;
        if (column == Constants::Prescription::Note) {
            drug->setPrescriptionValue(column, value.toString().replace("[","{").replace("]","}"));
            m_IsDirty = true;
        } else {
            drug->setPrescriptionValue(column, value);
            m_IsDirty = true;
        }
        return true;
    }

    QVariant getIDrugData(const IDrug *drug, const int column) const
    {
        using namespace ::DrugsDB::Constants;
        switch (column)
        {
        case Drug::Denomination :       return drug->brandName();
        case Drug::DrugId :             return drug->drugId();
        case Drug::UIDs :                return drug->uids().join(";");
        case Drug::Form :               return drug->forms().join(", ");
        case Drug::Route :              return drug->routes().join(", ");
        case Drug::ATC :                return drug->atcCode();
        case Drug::IsScoredTablet :     return drug->isScoredTablet();
        case Drug::GlobalStrength :     return drug->strength();
        case Drug::Molecules :          return drug->listOfMolecules();
        case Drug::AllInnsKnown :       return drug->data(IDrug::AllInnsKnown);
        case Drug::Inns :               return drug->listOfInn();
        case Drug::InnsATCcodes :       return drug->allAtcCodes();
        case Drug::MainInnCode :        return drug->mainInnCode();
        case Drug::MainInnDosage :      return drug->mainInnDosage();
        case Drug::MainInnName :        return drug->mainInnName();
        case Drug::InnClasses :         return drug->listOfInteractingClasses();
        case Drug::Administration :     return QVariant();
        case Drug::Interacts :          return m_InteractionResult->drugHaveInteraction(drug);
//        case Drug::MaximumLevelOfInteraction : return int(m_InteractionsManager->getMaximumTypeOfIAM(drug));
        case Drug::CompositionString :  return drug->toHtml();
        case Drug::InnCompositionString :  return drug->innComposition();
        case Drug::CodeMoleculesList :
            {
                QVariantList list;
                foreach(int code, drug->molsIds())
                    list << code;
                return list;
            }
        case Drug::HasPrescription :
            {
                const DrugsData *pres = static_cast<const DrugsData*>(drug);
                if (pres)
                    return pres->hasPrescription();
                return false;
            }
        case Drug::LinkToSCP : return drug->linkToSCP();
        case Drug::AvailableRoutes : return drug->routes();
        case Drug::AvailableForms :
            {
                QStringList toReturn;
                toReturn << drug->forms();
                toReturn << tkTr(Trans::Constants::INTAKES);
                if (drug->numberOfInn() == 1) {
                    toReturn << QApplication::translate("DrugsModel", "x %1 of %2")
                            .arg(drug->mainInnDosage())
                            .arg(drug->mainInnName());
                }
                return toReturn;
            }
        case Drug::AvailableDosages :
            {
                QStringList list;
                list << QApplication::translate("DrugsModel","Available Dosages");
                /** \todo add Drugs available dosage */
                return list.join("<br />");
                break;
            }
        case Drug::FullPrescription :
            {
                const DrugsData *pres = static_cast<const DrugsData*>(drug);
                if (!pres)
                    return QVariant();
                if (pres->prescriptionValue(Prescription::OnlyForTest).toBool() || m_SelectionOnlyMode) {
                    if (pres->prescriptionValue(Prescription::IsINNPrescription).toBool())
                        return pres->innComposition() + " [" + tkTr(Trans::Constants::INN) + "]";
                    else return pres->brandName();
                }
                return ::DrugsDB::DrugsModel::getFullPrescription(drug,false);
            }
        case Drug::OwnInteractionsSynthesis:
            {
                QVector<IDrugInteraction *> list = m_InteractionResult->getInteractions(drug);
                return interactionManager()->drugInteractionSynthesisToHtml(drug, list, false);
            }
        }
        return QVariant();
    }

    QVariant getPrescriptionData(const IDrug *drug, const int column) const
    {
        using namespace ::DrugsDB::Constants;
        Q_ASSERT(drug);
        if (!drug)
            return QVariant();
        if (column ==  Prescription::ToHtml) {
            return ::DrugsDB::DrugsModel::getFullPrescription(drug,true);
        } else {
            return drug->prescriptionValue(column);
        }
        return QVariant();
    }

    QVariant getInteractionData(const IDrug *drug, const int column) const
    {
        using namespace ::DrugsDB::Constants;
        switch (column)
        {
        case Interaction::Id :     return QVariant();
        case Interaction::Icon :   return m_InteractionResult->maxLevelOfInteractionIcon(drug, m_levelOfWarning);
        case Interaction::Pixmap : return m_InteractionResult->maxLevelOfInteractionIcon(drug, m_levelOfWarning).pixmap(16,16);
        case Interaction::MediumPixmap : return m_InteractionResult->maxLevelOfInteractionIcon(drug, m_levelOfWarning, Core::ITheme::MediumIcon).pixmap(64,64);
        case Interaction::ToolTip :
            {
                QString display;
                if (m_InteractionResult->drugHaveInteraction(drug)) {
                    display.append(interactionManager()->listToHtml(m_InteractionResult->getInteractions(drug), false));
                } else if (drug->data(IDrug::AllInnsKnown).toBool()) {
                    display = drug->listOfInn().join("<br />") + "<br />" + drug->listOfInteractingClasses().join("<br />");
                } else {
                    display = tkTr(Trans::Constants::NO_1_FOUND).arg(tkTr(Trans::Constants::INN));
                }
                return display;
            }
        case Interaction::FullSynthesis :
            {
                QString display;
                QVector<IDrugInteraction *> list = m_InteractionResult->interactions();
                int i = 0;
                display.append("<p>");
                foreach(IDrug *drg, m_DrugsList) {
                    ++i;
                    display.append(QString("%1&nbsp;.&nbsp;%2<br />")
                                   .arg(i)
                                   .arg(drg->brandName()));
                }
                display.append("</p><p>");
                if (list.count() > 0) {
                    display.append(interactionManager()->synthesisToHtml(list, true));
                } else
                    display = tkTr(Trans::Constants::NO_1_FOUND).arg(tkTr(Trans::Constants::INTERACTION));
                display.append("</p>");
                return display;
            }
        }
        return QVariant();
    }

    QVariant getDrugValue(const IDrug *drug, const int column) const
    {
        using namespace ::DrugsDB::Constants;
        if (column < Drug::MaxParam) {
            return getIDrugData(drug,column);
        } else if (column < Prescription::MaxParam) {
            return getPrescriptionData(drug, column);
        } else if (column < Interaction::MaxParam) {
            return getInteractionData(drug,column);
        }
        return QVariant();
    }

public:
    QList<IDrug *> m_DrugsList;
    QList<IDrug *> m_TestingDrugsList;
    int m_levelOfWarning;
    mutable QHash<int, QPointer<DosageModel> > m_DosageModelList;
    IDrug *m_LastDrugRequiered;
    bool m_ShowTestingDrugs, m_SelectionOnlyMode, m_IsDirty;
    DrugInteractionResult *m_InteractionResult;
    DrugInteractionQuery *m_InteractionQuery;
};
}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

/** \brief Constructor */
DrugsModel::DrugsModel(QObject * parent)
        : QAbstractTableModel(parent),
	d(new Internal::DrugsModelPrivate())
{
    static int handler = 0;
    ++handler;
    setObjectName("DrugsModel_" + QString::number(handler));
    if (!drugsBase()->isInitialized())
        Utils::Log::addError(this,"Drugs database not intialized", __FILE__, __LINE__);
    d->m_DrugsList.clear();
    d->m_DosageModelList.clear();

    // Make sure that the model always got a valid DrugInteractionResult and DrugInteractionQuery pointer
    d->m_InteractionQuery = new DrugInteractionQuery(this);
    d->m_InteractionQuery->setTestDrugDrugInteractions(true);
    d->m_InteractionQuery->setTestPatientDrugInteractions(true);

    d->m_InteractionResult = interactionManager()->checkInteractions(d->m_InteractionQuery);
    connect(drugsBase(), SIGNAL(dosageBaseHasChanged()), this, SLOT(dosageDatabaseChanged()));
}

/** \brief Destructor */
DrugsModel::~DrugsModel()
{
    if (d) delete d;
    d=0;
}

void DrugsModel::dosageDatabaseChanged()
{
    qDeleteAll(d->m_DosageModelList);
    d->m_DosageModelList.clear();
}

/** \brief count the number of selected drugs */
int DrugsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->m_DrugsList.count();
}

QModelIndex DrugsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row < d->m_DrugsList.count())
        return createIndex(row, column);
    return QModelIndex();
}

/**
  \brief Defines the data of drugs.
  The drugs model is a read only model for all the namespace mfDrugsConstants::Drug enum values.\n
  Dosages values are not accessible from here. Use the mfDosageModel.\n
  Prescritions values are writables. Informations are transmitted using DrugsDB::setPrescriptionValue().
*/
bool DrugsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    if (!index.isValid())
        return false;
    int row = index.row();
    if ((row >= d->m_DrugsList.count()) || (row < 0))
        return false;
    IDrug *drug = d->m_DrugsList.at(row);
    if (d->setDrugData(drug, index.column(), value)) {
        Q_EMIT dataChanged(index, index);
        QModelIndex fullPrescr = this->index(index.row(), Constants::Drug::FullPrescription);
        Q_EMIT dataChanged(fullPrescr, fullPrescr);
        Q_EMIT prescriptionResultChanged(getFullPrescription(drug,false));
    }
    return true;
}

/**
  \brief Set data for the specified drug with the corresponding \e CIS.
  Drug must be setted into the model otherwise, this function returns false.\n
  If you want the model to be refreshed call resetModel() after all datas were setted.
*/
bool DrugsModel::setDrugData(const QVariant &drugId, const int column, const QVariant &value)
{
    IDrug *drug = d->getDrug(drugId);
    if (!drug)
        return false;
    if (d->setDrugData(drug, column, value)) {
        Q_EMIT prescriptionResultChanged(getFullPrescription(drug,false));
        return true;
    }
    return false;
}

/** \brief Reset the model */
void DrugsModel::resetModel()
{
    reset();
}

/**
  \brief Returns the values of drugs and prescription, you can not access to the dosage model this way.
  mfDosageModel regarding a specific drug is accessible using dosageModel().\n
  Available datas index :
  \li Drugs specific datas : see mfDrugsConstants::Drug enumerator
  \li Interactions specific datas : see mfInteractionsConstants::Interaction enumerator
  \li Prescription datas : see mfDrugsConstants::Prescription enumerator
*/
QVariant DrugsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((index.row() > d->m_DrugsList.count()) || (index.row() < 0))
        return QVariant();

    const IDrug *drug = d->m_DrugsList.at(index.row());

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        int col = index.column();
        // manage indexes for dosageModel
        if ((col >= Dosages::Constants::Id) && (col < Dosages::Constants::MaxParam)) {
            return QVariant();
        } else {
            // deep in the abyss
            return d->getDrugValue(drug, index.column());
        }
    }
    else if (role == Qt::DecorationRole) {
        // Show/Hide interaction icon
        if (!settings()->value(Constants::S_SHOWICONSINPRESCRIPTION).toBool())
            return QVariant();

        if (drug->prescriptionValue(Constants::Prescription::IsTextualOnly).toBool()) {
            return theme()->icon(Core::Constants::ICONPENCIL);
        } else if (d->m_InteractionResult->drugHaveInteraction(drug)) {
            return d->m_InteractionResult->maxLevelOfInteractionIcon(drug, d->m_levelOfWarning);
        } else if (drug->data(IDrug::AllInnsKnown).toBool()) {
            return theme()->icon(Core::Constants::ICONOK);
        } else {
            return theme()->icon(Constants::INTERACTION_ICONUNKONW);
        }
    }
    else if (role == Qt::ToolTipRole) {
        QString display;
        if (GlobalDrugsModel::hasAllergy(drug)) {
            display += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                   .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONFORBIDDEN))
                   .arg(tr("KNOWN ALLERGY"));
        }
        display += drug->toHtml();
        if (d->m_InteractionResult->drugHaveInteraction(drug)) {
            QVector<IDrugInteraction *> list = d->m_InteractionResult->getInteractions(drug);
            display.append("<br>\n");
            display.append(interactionManager()->listToHtml(list, false));
        }
        return display;
    }
    else if (role == Qt::BackgroundRole) {
        // Drug is a 100% ?
        if (drug->prescriptionValue(Constants::Prescription::IsALD).toBool())
            return QColor(ALD_BACKGROUND_COLOR);
        // Drugs only for testings
        if (drug->prescriptionValue(Constants::Prescription::OnlyForTest).toBool())
            return QColor(FORTEST_BACKGROUND_COLOR);
        // Allergy / Intolerances
        if (GlobalDrugsModel::hasAllergy(drug))
            return QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString());
        if (GlobalDrugsModel::hasIntolerance(drug))
            return QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString());
    }
    else if (role == Qt::ForegroundRole) {
        if (drug->prescriptionValue(Constants::Prescription::OnlyForTest).toBool())
            return QColor(FORTEST_FOREROUND_COLOR);
    }
    return QVariant();
}

/**
  \brief At anytime, you can get all values of drugs inside the prescription model using the CIS as row index.
  \sa data()
*/
QVariant DrugsModel::drugData(const QVariant &drugId, const int column)
{
    IDrug *drug = d->getDrug(drugId);
    if (!drug)
        return QVariant();
    return d->getDrugValue(drug, column);
}

/** \brief Should not be used. \internal */
Qt::ItemFlags DrugsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;
}

/** \brief Removes \e count drugs from the \e row. */
bool DrugsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_LastDrugRequiered = 0;
    beginRemoveRows(parent, row, row+count);
    if (row >= d->m_DrugsList.count())
        return false;
    if ((row + count) > d->m_DrugsList.count())
        return false;
    int i;
    bool toReturn = true;
    for(i = 0; i < count; ++i) {
        IDrug *drug =  d->m_DrugsList.at(row+i);
        if ((!d->m_DrugsList.removeOne(drug)) && (!d->m_TestingDrugsList.removeOne(drug)))
            toReturn = false;
        d->m_InteractionQuery->removeDrug(drug);
        delete drug;
        drug = 0;
    }
    checkInteractions();
    endRemoveRows();
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
    return toReturn;
}


/**
 \brief Add a textual drug to the prescription.
 \sa DrugsWidget::TextualPrescriptionDialog, DrugsWidget::Internal::DrugSelector
*/
int DrugsModel::addTextualPrescription(const QString &drugLabel, const QString &drugNote)
{
    Internal::TextualDrugsData *drug = new Internal::TextualDrugsData();
    drug->setDenomination(drugLabel);
    drug->setPrescriptionValue(Constants::Prescription::Note, drugNote);
    drug->setPrescriptionValue(Constants::Prescription::IsTextualOnly, true);
    d->m_DrugsList << drug;
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
    return d->m_DrugsList.indexOf(drug);
}

/**
 \brief Add a drug to the prescription.
 \sa addDrug()
*/
int DrugsModel::addDrug(IDrug *drug, bool automaticInteractionChecking)
{
    if (!drug)
        return -1;
    // insert only once the same drug
    if (containsDrug(drug->uids()))
        return d->m_DrugsList.indexOf(drug);
    d->m_DrugsList << drug;
    d->m_InteractionQuery->addDrug(drug);
    // check drugs interactions ?
    if (automaticInteractionChecking) {
        d->m_levelOfWarning = settings()->value(Constants::S_LEVELOFWARNING).toInt();
    }
    checkInteractions();
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
    return d->m_DrugsList.indexOf(drug);
}

/**
  \brief Add a drug to the prescription.
  \e automaticInteractionChecking can be setted of if you want to add
     multiple drugs. You should call checkInteractions() after all in this case. \n
   Please take care, that the same drug can not be inserted more than once ! \n
   Return the index of the inserted drug into the list or -1 if no drug was inserted.
   \sa addDrug()
*/
int DrugsModel::addDrug(const QVariant &drugId, bool automaticInteractionChecking)
{
    return addDrug(drugsBase()->getDrugByDrugId(drugId), automaticInteractionChecking);
}

/**
  \brief Clear the prescription. Clear all interactions too.
  Calling this causes a model reset.
*/
void DrugsModel::clearDrugsList()
{
    d->m_LastDrugRequiered = 0;
    qDeleteAll(d->m_DrugsList);
    d->m_DrugsList.clear();
    qDeleteAll(d->m_TestingDrugsList);
    d->m_TestingDrugsList.clear();
    d->m_InteractionQuery->clearDrugsList();
    d->m_InteractionResult->clear();
    d->m_levelOfWarning = settings()->value(Constants::S_LEVELOFWARNING).toInt();
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
}

/**
  \brief Insert a list of drugs and check interactions.
  Calling this causes a model reset.
*/
void DrugsModel::setDrugsList(const QList<IDrug *> &list)
{
    clearDrugsList();
    d->m_DrugsList = list;
    d->m_InteractionQuery->setDrugsList(list.toVector());
    d->m_levelOfWarning = settings()->value(Constants::S_LEVELOFWARNING).toInt();
    checkInteractions();
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
}

/** \brief Returns the actual selected drugs list in the model */
const QList<IDrug *> &DrugsModel::drugsList() const
{
    return d->m_DrugsList;
}

/** \brief Returns true if the drug is already in the prescription */
bool DrugsModel::containsDrug(const QVariant &drugId) const
{
    if (d->getDrug(drugId))
        return true;
    return false;
}

/** \brief direct access to the DrugsData pointer. The pointer MUST BE DELETED. */
IDrug *DrugsModel::getDrug(const QVariant &drugUid) const
{
    return d->getDrug(drugUid);
}

/** \brief Returns true if the actual prescription has interaction(s). */
bool DrugsModel::prescriptionHasInteractions()
{
    return (d->m_InteractionResult->interactions().count()>0);
}

bool DrugsModel::prescriptionHasAllergies()
{
    foreach(const IDrug *drug, d->m_DrugsList) {
        if (GlobalDrugsModel::hasAllergy(drug))
            return true;
    }

    return false;
}

/**
  \brief Sort the drugs inside prescription. \sa DrugsDB::lessThan().
  Calling this causes a model reset.
*/
void DrugsModel::sort(int, Qt::SortOrder)
{
    qSort(d->m_DrugsList.begin(), d->m_DrugsList.end(), IDrug::lessThan);
    reset();
}

/**
  \brief Moves a drug up.
  Calling this causes a model reset.
*/
bool DrugsModel::moveUp(const QModelIndex &item)
{
    if (!item.isValid())
        return false;

    if (item.row() >= 1) {
        d->m_DrugsList.move(item.row(), item.row()-1);
        reset();
        return true;
    }
    return false;
}

/**
  \brief Moves a drug down.
  Calling this causes a model reset.
*/
bool DrugsModel::moveDown(const QModelIndex &item)
{
    if (!item.isValid())
        return false;

    if (item.row() < (rowCount()-1)) {
        d->m_DrugsList.move(item.row(), item.row()+1);
        reset();
        return true;
    }
    return false;
}

/**
  \brief Defines the model to show or hide the drugs only used for interaction testing only.
  Calling this causes a model reset.
*/
void DrugsModel::showTestingDrugs(bool state)
{
   if (state) {
       foreach(IDrug *drug, d->m_TestingDrugsList) {
            if (!d->m_DrugsList.contains(drug))
                d->m_DrugsList << drug;
        }
        d->m_TestingDrugsList.clear();
    } else {
        foreach(IDrug *drug, d->m_DrugsList) {
            if (!drug->prescriptionValue(Constants::Prescription::OnlyForTest).toBool())
                continue;
            if (!d->m_TestingDrugsList.contains(drug))
                d->m_TestingDrugsList << drug;
            d->m_DrugsList.removeOne(drug);
        }
    }
    d->m_ShowTestingDrugs = state;
    d->m_InteractionQuery->setDrugsList(d->m_DrugsList.toVector());
    checkInteractions();
    reset();
}

bool DrugsModel::testingDrugsAreVisible() const
{
    return d->m_ShowTestingDrugs;
}

void DrugsModel::setSelectionOnlyMode(bool b)
{
    d->m_SelectionOnlyMode = b;
    reset();
}

bool DrugsModel::isSelectionOnlyMode() const
{
    return d->m_SelectionOnlyMode;
}

void DrugsModel::setModified(bool state)
{
    d->m_IsDirty = state;
}

bool DrugsModel::isModified() const
{
    return d->m_IsDirty;
}


/** \brief Returns the dosage model for the selected drug */
Internal::DosageModel *DrugsModel::dosageModel(const QVariant &drugId)
{
//    if (! d->m_DosageModelList.keys().contains(uid)) {
//        d->m_DosageModelList.insert(uid, new Internal::DosageModel(this));
//        d->m_DosageModelList[uid]->setDrugUID(uid);
//    } else if (! d->m_DosageModelList.value(uid)) {
//        d->m_DosageModelList.insert(uid, new Internal::DosageModel(this));
//        d->m_DosageModelList[uid]->setDrugUID(uid);
//    }
//    return d->m_DosageModelList.value(uid);
    Internal::DosageModel *m = new Internal::DosageModel(this);
    m->setDrugId(drugId);
    return m;
}

/** \brief Returns the dosage model for the selected drug */
Internal::DosageModel *DrugsModel::dosageModel(const QModelIndex &drugIndex)
{
    if (!drugIndex.isValid())
        return 0;
    if (drugIndex.column() != Constants::Drug::DrugId)
        return 0;
    return dosageModel(drugIndex.data());
}

/** \brief Removes a drug from the prescription */
int DrugsModel::removeDrug(const QVariant &drugId)
{
    // Take care that this function removes all occurence of the referenced drug
    d->m_LastDrugRequiered = 0;
    d->m_InteractionQuery->clearDrugsList();
    int i = 0;
    foreach(IDrug * drug, d->m_DrugsList) {
        if (drug->drugId() == drugId) {
            d->m_DrugsList.removeAt(d->m_DrugsList.indexOf(drug));
            delete drug;
            ++i;
        } else {
            d->m_InteractionQuery->addDrug(drug);
        }
    }
    checkInteractions();
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
    return i;
}

/** \brief Removes last inserted drug from the prescription */
int DrugsModel::removeLastInsertedDrug()
{
    // TODO Take care if user inserted x times the same drug
    d->m_LastDrugRequiered = 0;
    if (d->m_DrugsList.count() == 0)
        return 0;
    delete d->m_DrugsList.last();
    d->m_DrugsList.removeLast();
    d->m_InteractionQuery->setDrugsList(d->m_DrugsList.toVector());
    checkInteractions();
    reset();
    d->m_IsDirty = true;
    Q_EMIT numberOfRowsChanged();
    return 1;
}

/**  \brief Only for debugging purpose. */
void DrugsModel::warn()
{
    if (!Utils::isDebugCompilation())
        return;
    qWarning() << "drugs in memory" << d->m_DrugsList.count();
    qWarning() << "dosagemodels in memory" << d->m_DosageModelList.count();
}

/** \brief Starts the interactions checking */
void DrugsModel::checkInteractions() const
{
    if (d->m_InteractionResult)
        delete d->m_InteractionResult;
    d->m_InteractionResult = interactionManager()->checkInteractions(*d->m_InteractionQuery);
}

QString DrugsModel::getFullPrescription(const IDrug *drug, bool toHtml, const QString &mask)
{
    QString tmp;
    if (mask.isEmpty()) {
        if (!toHtml)
            tmp = settings()->value(Constants::S_PRESCRIPTIONFORMATTING_PLAIN).toString();
        else
            tmp = settings()->value(Constants::S_PRESCRIPTIONFORMATTING_HTML).toString();
    }
    else
        tmp = mask;

    // Insert here __all__ tokens to the hash
    QHash<QString, QString> tokens_value;
    tokens_value.insert("DRUG", "");
    tokens_value.insert("Q_FROM", "");
    tokens_value.insert("Q_TO", "");
    tokens_value.insert("Q_SCHEME", "");
    tokens_value.insert("DAILY_SCHEME", "");
    tokens_value.insert("REPEATED_DAILY_SCHEME", "");
    tokens_value.insert("DISTRIBUTED_DAILY_SCHEME", "");
    tokens_value.insert("PERIOD", "");
    tokens_value.insert("PERIOD_SCHEME", "");
    tokens_value.insert("ROUTE", "");
    tokens_value.insert("D_FROM", "");
    tokens_value.insert("D_TO", "");
    tokens_value.insert("D_SCHEME", "");
    tokens_value.insert("MEAL", "");
    tokens_value.insert("NOTE", "");
    tokens_value.insert("MIN_INTERVAL", "");
    tokens_value.insert("MIN_INTERVAL_SCHEME", "");

    // Manage Textual drugs only
    if (drug->prescriptionValue(Constants::Prescription::IsTextualOnly).toBool()) {
        if (toHtml) {
            tokens_value["DRUG"] = drug->brandName().replace("\n","<br />");
            tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString().replace("\n","<br />");
        } else {
            tokens_value["DRUG"] = drug->brandName();
            tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString();
        }
        Utils::replaceTokens(tmp, tokens_value);
        return tmp;
    }

    // Manage full prescriptions
    if (drug->prescriptionValue(Constants::Prescription::IsINNPrescription).toBool()) {
        tokens_value["DRUG"] = drug->innComposition() + " [" + tkTr(Trans::Constants::INN) + "]";
    } else {
        tokens_value["DRUG"] =  drug->brandName();
    }

    if (drug->prescriptionValue(Constants::Prescription::IntakesFrom).toDouble()) {
        tokens_value["Q_FROM"] = QString::number(drug->prescriptionValue(Constants::Prescription::IntakesFrom).toDouble());
        if (drug->prescriptionValue(Constants::Prescription::IntakesUsesFromTo).toBool())
            tokens_value["Q_TO"] = QString::number(drug->prescriptionValue(Constants::Prescription::IntakesTo).toDouble());

        tokens_value["Q_SCHEME"] = drug->prescriptionValue(Constants::Prescription::IntakesScheme).toString();
    } else {
        tokens_value["Q_FROM"] = tokens_value["Q_TO"] = tokens_value["Q_SCHEME"] = "";
    }

    // Manage Daily Scheme See DailySchemeModel::setSerializedContent
    DrugsDB::DailySchemeModel *day = new DrugsDB::DailySchemeModel;
    day->setSerializedContent(drug->prescriptionValue(Constants::Prescription::DailyScheme).toString());
    tokens_value["REPEATED_DAILY_SCHEME"] = day->humanReadableRepeatedDailyScheme();
    tokens_value["DISTRIBUTED_DAILY_SCHEME"] = day->humanReadableDistributedDailyScheme();
    delete day;
    if (tokens_value.value("REPEATED_DAILY_SCHEME").isEmpty()) {
        tokens_value["DAILY_SCHEME"] = tokens_value.value("DISTRIBUTED_DAILY_SCHEME");
    } else {
        tokens_value["DAILY_SCHEME"] = tokens_value.value("REPEATED_DAILY_SCHEME");
    }

    // Duration
    if (drug->prescriptionValue(Constants::Prescription::DurationFrom).toDouble()) {
        tokens_value["D_FROM"] = QString::number(drug->prescriptionValue(Constants::Prescription::DurationFrom).toDouble());
        if (drug->prescriptionValue(Constants::Prescription::DurationUsesFromTo).toBool())
            tokens_value["D_TO"] = QString::number(drug->prescriptionValue(Constants::Prescription::DurationTo).toDouble());

        // Manage plurial form
        tokens_value["PERIOD_SCHEME"] = drug->prescriptionValue(Constants::Prescription::PeriodScheme).toString();
        tokens_value["D_SCHEME"] = drug->prescriptionValue(Constants::Prescription::DurationScheme).toString();
        int max = qMax(drug->prescriptionValue(Constants::Prescription::DurationFrom).toDouble(), drug->prescriptionValue(Constants::Prescription::DurationTo).toDouble());
        if (periods().contains(tokens_value["D_SCHEME"])) {
            tokens_value["D_SCHEME"] = periodPlurialForm(periods().indexOf(tokens_value["D_SCHEME"]), max, tokens_value["D_SCHEME"]);
        }
        max = drug->prescriptionValue(Constants::Prescription::Period).toDouble();
        if (max==1 && QLocale().name().left(2)=="fr")
            ++max;
        if (periods().contains(tokens_value["PERIOD_SCHEME"])) {
            tokens_value["PERIOD_SCHEME"] = periodPlurialForm(periods().indexOf(tokens_value["PERIOD_SCHEME"]), max, tokens_value["PERIOD_SCHEME"]);
        }
    } else {
        tokens_value["PERIOD_SCHEME"] = tokens_value["D_FROM"] = tokens_value["D_TO"] = tokens_value["D_SCHEME"] = "";
    }

    tokens_value["MEAL"] = Trans::ConstantTranslations::mealTime(drug->prescriptionValue(Constants::Prescription::MealTimeSchemeIndex).toInt());
    QString tmp2 = drug->prescriptionValue(Constants::Prescription::Period).toString();

    /** \todo provide a better management of 'EACH DAY__S__' here .. \sa Translation::periodPlurialForm()*/
    // Period management of 'EACH DAY__S'
    if (tmp2 == "1") {
        tmp2.clear();
    }
    tokens_value["PERIOD"] = tmp2;

    if (toHtml) {
        tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString().replace("\n","<br />");
    } else {
        tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString();
    }

    // Min interval
    const QVariant &interval = drug->prescriptionValue(Constants::Prescription::IntakesIntervalOfTime);
    const QVariant &intervalScheme = drug->prescriptionValue(Constants::Prescription::IntakesIntervalScheme);
    if ((!interval.isNull() && !intervalScheme.isNull()) &&
        interval.toInt() > 0) {
        tokens_value["MIN_INTERVAL"] = interval.toString() + " " + periodPlurialForm(intervalScheme.toInt(), interval.toInt());
    }

    // Route
    tokens_value["ROUTE"] = drug->prescriptionValue(Constants::Prescription::Route).toString();

    Utils::replaceTokens(tmp, tokens_value);
    return tmp;
}

Qt::DropActions DrugsModel::supportedDropActions() const
{
#if QT_VERSION >= 0x040600
    return Qt::CopyAction;
#else
    return Qt::MoveAction | Qt::CopyAction;
#endif
}

QStringList DrugsModel::mimeTypes() const
{
    return QStringList() << Templates::Constants::MIMETYPE_TEMPLATE;// DrugsDB::DrugsIO::prescriptionMimeTypes();
//    return QStringList() << DrugsDB::DrugsIO::prescriptionMimeTypes();
}

//QMimeData *DrugsModel::mimeData(const QModelIndexList &indexes) const
//{
//
//}

bool DrugsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
//    if (action == Qt::MoveAction)
//        qWarning() << "DrugsModel::dropMimeData Move" << row << data->data(mimeTypes().at(0));
//    else if (action == Qt::CopyAction)
//        qWarning() << "DrugsModel::dropMimeData Copy" << row << data->data(mimeTypes().at(0));

    if (action == Qt::IgnoreAction)
        return true;

    // only one template can be added once
    if (data->data(mimeTypes().at(0)).contains(";"))
        return false;

    // get index from the transmitted ids
    Templates::TemplatesModel *model = new Templates::TemplatesModel(this);

    QList<QPersistentModelIndex> list = model->getIndexesFromMimeData(data);

    foreach(const QPersistentModelIndex &idx, list) {
        // do not accept templates with children (that should never be the case)
        if (model->hasChildren(idx))
            continue;
        if (model->isCategory(idx))
            continue;
        // add content to model
        DrugsDB::DrugsIO::prescriptionFromXml(this, model->index(idx.row(), Templates::Constants::Data_Content, idx.parent()).data().toString(), DrugsDB::DrugsIO::AppendPrescription);
    }

    // never move templates but copy them
    if (action == Qt::MoveAction)
        return false;

    return true;
}
