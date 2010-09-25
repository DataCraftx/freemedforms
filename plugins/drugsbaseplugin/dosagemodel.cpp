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
  \class DosageModel
  \brief Manages predetermined dosage.
  A \b "dosage" is a set of values that defines what, when and how to prescribe a drug. A dosage can apply :
  \li on a specific drug base on its CIS,
  \li on the INN molecule of the drug which are presented in the same "dosage" (100mg, 1g...).
  For example :
  \li CLAMOXYL 1g can have prescription based on its CIS or on the \e amoxicilline molecule.
  \li CIS based could be : one intake morning and evening during seven days.
  \li INN based could be the same and can be applied to AMOXICILLINE PHARMACEUTICAL_LABO 1g.

  A \b "prescription" is the when and how you prescribe a selected drug.\n
  database(DB_DOSAGES_NAME) should be defined BEFORE instance()

  \todo Create a specific user's right for dosage creation/edition/modification/deletion +++.
  \todo Get user's right throught Core::IUser

  \ingroup freediams drugswidget
*/


#include "dosagemodel.h"

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>

/** \todo reimplement user management */

#include <QSqlRecord>
#include <QSqlError>
#include <QApplication>
#include <QStringList>
#include <QUuid>
#include <QDateTime>
#include <QColor>
#include <QLocale>

enum Warn { WarnDebuggingDatas = false };

/**
  \todo remove QCache of DosageModels
*/

static inline Core::IUser *user() {return Core::ICore::instance()->user();}



namespace mfDosageModelConstants {
    const char *const XML_DOSAGE_MAINTAG            = "DOSAGE";
    const char* const DIRTYROW_BACKGROUNDCOLOR      = "yellow";
}

using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;
using namespace mfDosageModelConstants;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Static Datas --------------------------------------------
//--------------------------------------------------------------------------------------------------------
// intialize static private members
QStringList DosageModel::m_ScoredTabletScheme = QStringList();
QStringList DosageModel::m_PreDeterminedForms = QStringList();
QString     DosageModel::m_ActualLangage = "";


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Translations ----------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Used to retranslate static stringlists */
void DosageModel::changeEvent(QEvent * event)
{
    // proceed only LangageChange events
    if (event->type() != QEvent::LanguageChange)
        return;
    retranslate();
}

/** \brief Used to retranslate static stringlists */
void DosageModel::retranslate()
{
    // proceed only if needed
    if (m_ActualLangage == QLocale().name().left(2))
        return;

    // store the langage and retranslate
    m_ActualLangage = QLocale().name().left(2);
    m_ScoredTabletScheme.clear();
    m_PreDeterminedForms.clear();

    m_ScoredTabletScheme =
            QStringList()
            << tr("complet tab.")
            << tr("half tab.")
            << tr("quater tab.");

    m_PreDeterminedForms =
            QStringList()
            << tr("dose per kilograms")
            << tr("reference spoon")
            << tr("2.5 ml spoon")
            << tr("5 ml spoon")
            << tr("puffs")
            << tr("dose")
            << tr("mouthwash")
            << tr("inhalation")
            << tr("application")
            << tr("washing")
            << tr("shampooing")
            << tr("eyewash")
            << tr("instillation")
            << tr("pulverization");
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Static datas ----------------------------------------
//--------------------------------------------------------------------------------------------------------

/** \brief Scored tablet predetermined stringlist */
QStringList DosageModel::scoredTabletScheme()
{
    if (m_ScoredTabletScheme.count() == 0)
        retranslate();
    return m_ScoredTabletScheme;
}

QStringList DosageModel::predeterminedForms()
{
    if (m_PreDeterminedForms.count()==0)
        retranslate();
    return m_PreDeterminedForms;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ Dosage Model ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Constructor */
DosageModel::DosageModel(DrugsDB::DrugsModel *parent)
    : QSqlTableModel(parent, QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME)),
        m_DrugsModel(parent)
{
    setObjectName("DosageModel");
    QSqlTableModel::setTable(Dosages::Constants::DB_DOSAGES_TABLE_NAME);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_UID = -1;
    if (drugsBase()->isDatabaseTheDefaultOne()) {
        setFilter(QString("%1 = \"%2\"")
                  .arg(database().record(Dosages::Constants::DB_DOSAGES_TABLE_NAME).fieldName(Dosages::Constants::DrugsDatabaseIdentifiant))
                  .arg(DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT));
    } else {
        if (drugsBase()->actualDatabaseInformations()) {
            setFilter(QString("%1 = \"%2\"")
                      .arg(database().record(Dosages::Constants::DB_DOSAGES_TABLE_NAME).fieldName(Dosages::Constants::DrugsDatabaseIdentifiant))
                      .arg(drugsBase()->actualDatabaseInformations()->identifiant));
        }
    }
}

/** \brief Defines datas for the dosage. Database is only updated when calling submitAll(). */
bool DosageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::setData", "before using the dosagemodel, you must specify the UID of the related drug");
    if (! index.isValid())
        return false;
    if (role == Qt::EditRole || role == Qt::DisplayRole) {

        QVariant q = data(index);
        // verify the value is different as model
        if (q==value) {
            return true;
        } else if (q.isNull()) {
            if (value.toString().isEmpty())
                return true;
        }

        // set only once modification date (infinite loop prevention)
        if (index.column()!=Dosages::Constants::ModificationDate)
            setData(this->index(index.row(), Dosages::Constants::ModificationDate), QDateTime::currentDateTime());

        // mark row as dirty
        if (!m_DirtyRows.contains(index.row())) {
            m_DirtyRows << index.row();
        }

//        if  ((index.column() == Dosages::Constants::INN_LK) || (index.column() == Dosages::Constants::InnLinkedDosage)){
//            qWarning() << index.column() << value << m_DirtyRows;
//            warn();
//        }

        if (!QSqlTableModel::setData(index, value, role)) {
            Utils::Log::addError(this, "Can not set data to QSqlTableModel", __FILE__, __LINE__);
            Utils::Log::addQueryError(this, query(), __FILE__, __LINE__);
            return false;
        }

        QModelIndex label = this->index(index.row(), Dosages::Constants::Label);
        emit dataChanged(label, label);
    }
    return false;
}

/** \brief Retreive dosage's datas. */
QVariant DosageModel::data(const QModelIndex & item, int role) const
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::data", "before using the dosagemodel, you must specify the UID of the related drug");
    if (!item.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::FontRole:
        {
            QFont font;
            if (m_DirtyRows.contains(item.row()))
                font.setBold(true);
            else
                font.setBold(false);
            return font;
        }
    case Qt::BackgroundRole :
        {
            if (m_DirtyRows.contains(item.row()))
                return QColor(DIRTYROW_BACKGROUNDCOLOR);
            else
                return QColor("white");
        }
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            return QSqlTableModel::data(item, role);
            break;
        }
    case Qt::DecorationRole :
        {
            int t = QSqlTableModel::index(item.row(), Dosages::Constants::INN_LK).data().toInt();
//            qWarning() << t;
//            if (!t.isEmpty() && (t != "-1"))
            if (t > 0)
                return theme()->icon(DrugsDB::Constants::I_SEARCHINN);
            return theme()->icon(DrugsDB::Constants::I_SEARCHCOMMERCIAL);
        }
    }
    return QVariant();
}

/** \brief Create new dosages. Defaults values of the dosages are stted here. */
bool DosageModel::insertRows(int row, int count, const QModelIndex & parent)
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::insertRows", "before inserting row, you must specify the UID of the related drug");
    if (WarnDebuggingDatas)
        qWarning() << "DosageModel::insertRows (row:" << row << ";count" << count << ")" << parent;
    QString userUuid = user()->value(Core::IUser::Uuid).toString();
    int i;
    int createdRow;
    bool toReturn = true;
    for (i=0; i < count; ++i) {
        createdRow = row+i;
        if (!QSqlTableModel::insertRows(createdRow, 1, parent)) {
            Utils::Log::addError(this, tr("Model Error : unable to insert a row"),__FILE__, __LINE__);
            toReturn = false;
        } else {
            setData(index(createdRow, Dosages::Constants::Uuid) , QUuid::createUuid().toString());
            if (drugsBase()->actualDatabaseInformations())
                setData(index(createdRow, Dosages::Constants::DrugsDatabaseIdentifiant) , drugsBase()->actualDatabaseInformations()->identifiant);
            setData(index(createdRow, Dosages::Constants::DrugUid_LK) , m_UID);
            setData(index(createdRow, Dosages::Constants::INN_LK) , -1);
            setData(index(createdRow, Dosages::Constants::InnLinkedDosage) , "");
            setData(index(createdRow, Dosages::Constants::IntakesTo) , 1);
            setData(index(createdRow, Dosages::Constants::IntakesFrom) , 1);
            setData(index(createdRow, Dosages::Constants::IntakesUsesFromTo) , false);
            QString s = settings()->value(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA).toString();
            if (s.isEmpty()) {
                setData(index(createdRow, Dosages::Constants::IntakesScheme) , m_DrugsModel->drugData(m_UID, Drug::AvailableForms).toStringList().at(0));
            } else if (s=="||") {
                setData(index(createdRow, Dosages::Constants::IntakesScheme) , tkTr(Trans::Constants::INTAKES, 1));
            } else {
                setData(index(createdRow, Dosages::Constants::IntakesScheme) , s);
            }
            setData(index(createdRow, Dosages::Constants::Period), 1);
            setData(index(createdRow, Dosages::Constants::PeriodScheme) , tkTr(Trans::Constants::DAY_S));
            setData(index(createdRow, Dosages::Constants::DurationTo) , 1);
            setData(index(createdRow, Dosages::Constants::DurationFrom) , 1);
            setData(index(createdRow, Dosages::Constants::DurationUsesFromTo) , false);
            setData(index(createdRow, Dosages::Constants::DurationScheme) , tkTr(Trans::Constants::MONTH_S));
            setData(index(createdRow, Dosages::Constants::IntakesIntervalOfTime) , 0);
            setData(index(createdRow, Dosages::Constants::MinAge) , 0);
            setData(index(createdRow, Dosages::Constants::MaxAge) , 0);
            setData(index(createdRow, Dosages::Constants::MinWeight) , 0);
            setData(index(createdRow, Dosages::Constants::MinClearance) , 0);
            setData(index(createdRow, Dosages::Constants::MaxClearance) , 0);
            setData(index(createdRow, Dosages::Constants::SexLimitedIndex) , 0);
            setData(index(createdRow, Dosages::Constants::Note) , QVariant());
            setData(index(createdRow, Dosages::Constants::CreationDate) , QDateTime::currentDateTime());
            setData(index(createdRow, Dosages::Constants::Transmitted) , QVariant());
        }
    }
    return toReturn;
}

/**
  \brief Removes somes rows.
  \todo  when QSqlTableModel removes rows it may call select() and reset(),  this causes non submitted modifications deletion on the model THIS MUST BE IMPROVED
*/
bool DosageModel::removeRows(int row, int count, const QModelIndex & parent)
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::removeRows", "before using the dosagemodel, you must specify the UID of the related drug");
    if (WarnDebuggingDatas)
        qWarning() << "DosageModel::removeRows (row:" << row << ";count" << count << ")" << parent;

    if (row < 0)
        return false;

    setEditStrategy(QSqlTableModel::OnRowChange);
    bool toReturn = false;
    if (QSqlTableModel::removeRows(row, count, parent)) {
        int i;
        for(i=0; i < count; ++i) {
            if (m_DirtyRows.contains(row+i))
                m_DirtyRows.remove(row+i);
        }
        toReturn = true;
    }
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    return toReturn;
}

/** \brief Revert a row */
void DosageModel::revertRow(int row)
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::revertRow", "before using the dosagemodel, you must specify the UID of the related drug");
    QSqlTableModel::revertRow(row);
    if (m_DirtyRows.contains(row))
        m_DirtyRows.remove(row);
}

/** \brief Submit model changes to database */
bool DosageModel::submitAll()
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::submitAll", "before using the dosagemodel, you must specify the UID of the related drug");
//    warn();
//    qWarning() << m_DirtyInnLkRows << m_DirtyRows;
//    foreach(const int i, m_DirtyInnLkRows) {
//        this->revertRow(i);
//    }

    QSet<int> safe = m_DirtyRows;
    m_DirtyRows.clear();
    if (QSqlTableModel::submitAll()) {
        return true;
    } else {
        m_DirtyRows = safe;
        Utils::Log::addQueryError(this, QSqlTableModel::query());
    }
    //    reset();
    return false;
}

/**
  \brief Filter the model from the drug CIS and if possible for the inn prescriptions.
*/
bool DosageModel::setDrugUID(const QVariant &drugUid)
{
    if (drugUid == m_UID)
        return true;
    m_UID = drugUid;
    QString filter = QString("%1='%2'").arg(record().fieldName(Dosages::Constants::DrugUid_LK)).arg(m_UID.toString());

    int inn = -1;
    if (m_DrugsModel)
        inn = m_DrugsModel->drugData(drugUid, Constants::Drug::MainInnCode).toInt();

    if (inn!=-1) {
        // add INN_LK
        QString innFilter = QString::number(inn);
        innFilter = QString("%1=%2").arg(record().fieldName(Dosages::Constants::INN_LK)).arg(innFilter);
        // add INN_DOSAGE
        innFilter = QString("(%1) AND (%2='%3')")
                    .arg(innFilter)
                    .arg(record().fieldName(Dosages::Constants::InnLinkedDosage))
                    .arg(m_DrugsModel->drugData(drugUid, Constants::Drug::MainInnDosage).toString());
        filter = QString("((%1) OR (%2))").arg(filter).arg(innFilter);
    }        

    if (WarnDebuggingDatas)
        qWarning() << "DosageModel filter" << filter << __FILE__ << __LINE__;

    setFilter(filter);
    select();
    return true;
}

/** \brief Return the actual drug's CIS */
QVariant DosageModel::drugUID()
{
    return m_UID;
}

/** \brief Test all the column of the selected row, if one value is dirty return true. */
bool DosageModel::isDirty(const int row) const
{
    int i;
    for(i=0;i<columnCount();++i) {
        if (QSqlTableModel::isDirty(index(row, i)))
            return true;
    }
    return false;
}

/** \brief Check the validity of the dosage. */
QStringList DosageModel::isDosageValid(const int row)
{
    Q_ASSERT_X(!m_UID.isNull(), "DosageModel::isDosageValid", "before using the dosagemodel, you must specify the UID of the related drug");
    QStringList errors;
    // Label
    if (index(row, Dosages::Constants::Label).data().toString().isEmpty()) {
        QString label = m_DrugsModel->getFullPrescription(m_DrugsModel->getDrug(m_UID), false, Constants::PROTOCOL_AUTOMATIC_LABEL_MASK);
        setData(index(row, Dosages::Constants::Label), label);
    }
    // Intakes
    if (index(row, Dosages::Constants::IntakesScheme).data().toString().isEmpty())
        errors << tr("The intakes' scheme must be defined.");
    if (index(row, Dosages::Constants::PeriodScheme).data().toString().isEmpty())
        errors << tr("The period's scheme must be defined.");
    // Duration
    if (index(row, Dosages::Constants::DurationScheme).data().toString().isEmpty())
        errors << tr("The duration's scheme must be defined.");

    //TODO
    return errors;
}

/**
  \brief Transforms a dosage to a drug's prescription.
  \sa DrugsModel
*/
void DosageModel::toPrescription(const int row)
{
    Q_ASSERT(m_DrugsModel);
    Q_ASSERT(m_DrugsModel->containsDrug(m_UID));
    /** \todo add a mutext ? */
    QHash<int,int> prescr_dosage;
    prescr_dosage.insert(Constants::Prescription::UsedDosage ,           Dosages::Constants::Uuid);
    prescr_dosage.insert(Constants::Prescription::IntakesFrom ,          Dosages::Constants::IntakesFrom);
    prescr_dosage.insert(Constants::Prescription::IntakesTo ,            Dosages::Constants::IntakesTo);
    prescr_dosage.insert(Constants::Prescription::IntakesUsesFromTo ,    Dosages::Constants::IntakesUsesFromTo);
    prescr_dosage.insert(Constants::Prescription::IntakesScheme ,        Dosages::Constants::IntakesScheme);
    prescr_dosage.insert(Constants::Prescription::Period ,               Dosages::Constants::Period);
    prescr_dosage.insert(Constants::Prescription::PeriodScheme ,         Dosages::Constants::PeriodScheme);
    prescr_dosage.insert(Constants::Prescription::DurationFrom ,         Dosages::Constants::DurationFrom);
    prescr_dosage.insert(Constants::Prescription::DurationTo ,           Dosages::Constants::DurationTo);
    prescr_dosage.insert(Constants::Prescription::DurationUsesFromTo,    Dosages::Constants::DurationUsesFromTo);
    prescr_dosage.insert(Constants::Prescription::DurationScheme ,       Dosages::Constants::DurationScheme);
    prescr_dosage.insert(Constants::Prescription::IntakesIntervalOfTime ,Dosages::Constants::IntakesIntervalOfTime);
    prescr_dosage.insert(Constants::Prescription::IntakesIntervalScheme ,Dosages::Constants::IntakesIntervalScheme);
    prescr_dosage.insert(Constants::Prescription::Note ,                 Dosages::Constants::Note);
    prescr_dosage.insert(Constants::Prescription::DailyScheme ,          Dosages::Constants::DailyScheme);
    prescr_dosage.insert(Constants::Prescription::MealTimeSchemeIndex ,  Dosages::Constants::MealScheme);
    prescr_dosage.insert(Constants::Prescription::IsALD ,                Dosages::Constants::IsALD);
    foreach(const int i, prescr_dosage.keys()) {
        m_DrugsModel->setDrugData(m_UID, i, data(index(row, prescr_dosage.value(i))));
    }

    // Manage INN Prescriptions
    if (index(row,Dosages::Constants::INN_LK).data().toInt() > 999) // this is an INN prescription
        m_DrugsModel->setDrugData(m_UID, Constants::Prescription::IsINNPrescription, true);
    else
        m_DrugsModel->setDrugData(m_UID, Constants::Prescription::IsINNPrescription, false);

    // Reset model
    m_DrugsModel->resetModel();
}

/** \brief Transforms a model's row into XML encoded QString. This part is using the database fieldnames.*/
QString DosageModel::toXml(const int row)
{
    int j;
    QHash<QString,QString> datas;
    for(j=0; j < columnCount(); ++j) {
        datas.insert(record().fieldName(j).toLower(), index(row, j).data().toString());
    }
    return Utils::createXml(XML_DOSAGE_MAINTAG,datas,4,false);
}

/**
  \brief Adds a XML file to the model. A verification of the drugs' UID is done.
  \todo write this
*/
bool DosageModel::addFromXml(const QString &xml)
{
    /** \todo here */
    //    QHash<QString, QString> datas;
    //    int n = xml.count("<"+XML_DOSAGE_MAINTAG+">");
    //    if (n<=0)
    //        return false;
    //    foreach(const QString &s, xml.split("</"+XML_DOSAGE_MAINTAG+">")) {
    //        if (!Utils::readXml(xml,XML_DOSAGE_MAINTAG,datas,false))
    //            return false;
    //        // insert row, get the index of it
    //        foreach(const QString &k, datas.keys()) {
    //            int id = record().indexOf(k);
    //            if (id==-1)
    //                continue;
    //            //        setData(index(
    //        }
    //
    //    }
    return true;
}

/** \brief For debugging purpose only */
void DosageModel::warn(const int row)
{
    if (!Utils::isDebugCompilation())
        return;
    //    qWarning() << "database connection" << database().connectionName();

    // row == -1 -> warn all rows
    int i;
    int j;
    if (row == -1) {
        for (i=0; i < rowCount(); ++i)
            qWarning() << toXml(i);
        //            for(j=0; j < columnCount(); ++j) {
        //            qWarning() << record().fieldName(j) << index(i, j).data() ;
        //        }
    } else {
        for(j=0; j < columnCount(); ++j) {
            qWarning() << record().fieldName(j) << index(row, j).data() ;
        }
    }
}
