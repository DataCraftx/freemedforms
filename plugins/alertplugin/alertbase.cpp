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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Alert::Internal::AlertBase
  Alert database manager. For internal use.
  \sa Alert::AlertCore
*/

/**
  \class Alert::Internal::AlertBaseQuery
  Construct queries for the Alert::Internal::AlertBase.
*/

#include "alertbase.h"
#include "alertitem.h"
#include "constants.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/icommandline.h>

#include <QDir>
#include <QTreeWidget>
#include <QHash>
#include <QSqlQuery>

enum { WarnGetAlertQuerySQLCommand = false };

using namespace Alert;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

static inline bool connectDatabase(const QString &connectionName, const int line)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            Utils::Log::addError("AlertBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(db.connectionName()).arg(db.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

enum DbValues {
    ItemId = 0,
    RelatedId,
    CategoryUid,
    ScriptId,
    ValidationId,
    TimingId,
    LabelLID,
    CategoryLID,
    DescrLID,
    CommentLID
};

/**
 \enum Alert::AlertBaseQuery::AlertValidity
 Defines the required validity of alerts.

 \var Alert::AlertBaseQuery::ValidAlerts
 Valid alerts are alerts that are currently started but not yet expired or validated.

 \var Alert::AlertBaseQuery::InvalidAlerts
 Invalid alerts are alerts that are not currently started, or expired or validated.

 \var Alert::AlertBaseQuery::ValidAndInvalidAlerts
 Query both alerts.
*/

/**  Create an empty query on valid alerts. */
AlertBaseQuery::AlertBaseQuery() :
    _validity(AlertBaseQuery::ValidAlerts),
    _viewType(-1)
{
    _start = QDate::currentDate();
    _end = _start.addYears(1);
}

AlertBaseQuery::~AlertBaseQuery()
{}

/** Define the query to the unique alert item with the \e uuid. All other params are ignored. */
void AlertBaseQuery::getAlertItemFromUuid(const QString &uuid)
{
    _itemUid = uuid;
}

/** Return the queried alert item uuid. If an uuid is provided, all other params are ignored. */
QString AlertBaseQuery::alertItemFromUuid() const
{
    return _itemUid;
}

/** Define the validity of queried alerts \sa Alert::Internal::AlertBaseQuery::AlertValidity */
void AlertBaseQuery::setAlertValidity(AlertBaseQuery::AlertValidity validity)
{
    _validity = validity;
}

/** Return the queried validity \sa Alert::Internal::AlertBaseQuery::AlertValidity */
AlertBaseQuery::AlertValidity AlertBaseQuery::alertValidity() const
{
    return _validity;
}

/** Query alerts for the current logged user. */
void AlertBaseQuery::addCurrentUserAlerts()
{
    QString u;
    if (user()) {
        u = user()->uuid();
    } else {
        if (!Utils::isReleaseCompilation())
            u = "user1";
    }
    if (!_userUids.contains(u))
        _userUids << u;
}

/** Query alerts for the user according to its \e uuid. */
void AlertBaseQuery::addUserAlerts(const QString &uuid)
{
    if (!_userUids.contains(uuid))
        _userUids << uuid;
}

/** Query alerts for the current editing patient. */
void AlertBaseQuery::addCurrentPatientAlerts()
{
    QString u;
    if (patient()) {
        u = patient()->uuid();
    } else {
        if (!Utils::isReleaseCompilation())
            u = "patient1";
    }
    if (!_patientUids.contains(u))
        _patientUids << u;
}

/** Query alerts for the patient according to its \e uuid. */
void AlertBaseQuery::addPatientAlerts(const QString &uuid)
{
    if (!_patientUids.contains(uuid))
        _patientUids << uuid;
}

/** Query application alerts for the \e appName application. You can query multiple application at the same time. */
void AlertBaseQuery::addApplicationAlerts(const QString &appName)
{
    _appNames << appName;
}

/** Returns all queried user uuids. */
QStringList AlertBaseQuery::userUids() const
{
    return _userUids;
}

/** Returns all queried patient uuids. */
QStringList AlertBaseQuery::patientUids() const
{
    return _patientUids;
}

/** Returns all queried application names. */
QStringList AlertBaseQuery::applicationNames() const
{
    return _appNames;
}

/** Query alerts for a specific date range (\e start, \e end). */
void AlertBaseQuery::setDateRange(const QDate &start, const QDate &end)
{
    _start = start;
    _end = end;
}

/** Return true if a date is defined. */
bool AlertBaseQuery::dateRangeDefined() const
{
    return (_start.isValid() && _end.isValid());
}

/** Returns the date range. */
QDate AlertBaseQuery::dateRangeStart() const
{
    return _start;
}

/** Returns the date range. */
QDate AlertBaseQuery::dateRangeEnd() const
{
    return _end;
}

void AlertBaseQuery::setAlertViewType(AlertItem::ViewType viewType)
{
    _viewType = viewType;
}

/** Return -1 if nothing was defined */
AlertItem::ViewType AlertBaseQuery::alertViewType() const
{
    return AlertItem::ViewType(_viewType);
}

//void AlertBaseQuery::addCategory(const QString &category, const QString &lang = QString::null)
//{
//    if (lang.isEmpty())
//        _categories.insertMulti(lang, category);
//}

//QMultiHash<QString, QString> AlertBaseQuery::categories() const
//{}


AlertBase::AlertBase(QObject *parent) :
    QObject(parent),
    Utils::Database(),
    m_initialized(false)
{
    setObjectName("AlertBase");

    using namespace Alert::Constants;
    addTable(Table_ALERT, "ALR");
    addTable(Table_ALERT_LABELS, "LBL");
    addTable(Table_ALERT_RELATED, "REL");
    addTable(Table_ALERT_SCRIPTS, "SCR");
    addTable(Table_ALERT_TIMING, "TIM");
    addTable(Table_ALERT_VALIDATION, "VAL");
    addTable(Table_ALERT_PACKS, "PACKS");
    addTable(Table_ALERT_VERSION, "VER");

    addField(Table_ALERT, ALERT_ID, "A_ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT, ALERT_UID, "A_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_PACKUID, "A_PUID", FieldIsUUID);
    addField(Table_ALERT, ALERT_REL_ID, "R_ID", FieldIsUUID);
    addField(Table_ALERT, ALERT_CATEGORY_UID, "C_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_SID, "SCR_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_VAL_ID, "VAL_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_ISVALID, "ISV", FieldIsBoolean);
    addField(Table_ALERT, ALERT_ISREMINDABLE, "REMIND", FieldIsBoolean);

    addField(Table_ALERT, ALERT_VIEW_TYPE, "VIEW_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CONTENT_TYPE, "CONTENT_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_TIM_ID, "TIM_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CONDITION_TYPE, "COND_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_PRIORITY, "PRIOR", FieldIsInteger);
    addField(Table_ALERT, ALERT_OVERRIDEREQUIREUSERCOMMENT, "VRUC", FieldIsInteger);
    addField(Table_ALERT, ALERT_MUSTBEREAD, "MBR", FieldIsInteger);

    addField(Table_ALERT, ALERT_LABEL_LID, "LBL_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CATEGORY_LID, "CAT_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_DESCRIPTION_LID, "DES_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_COMMENT_LID, "COM_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CREATION_DATE, "C_DATE", FieldIsDate);
    addField(Table_ALERT, ALERT_LAST_UPDATE_DATE, "U_DATE", FieldIsDate);
    addField(Table_ALERT, ALERT_THEMED_ICON, "THM_ICON", FieldIsDate);
    addField(Table_ALERT, ALERT_THEME_CSS, "CSS", FieldIsLongText);
    addField(Table_ALERT, ALERT_CRYPTED_PASSWORD, "PASS", FieldIsShortText);
    addField(Table_ALERT, ALERT_EXTRA_XML, "EXTRA", FieldIsBlob);

    addIndex(Table_ALERT, ALERT_ID);
    addIndex(Table_ALERT, ALERT_UID);
    addIndex(Table_ALERT, ALERT_PACKUID);
    addIndex(Table_ALERT, ALERT_REL_ID);
    addIndex(Table_ALERT, ALERT_CATEGORY_UID);

    // Alert relations to user/patient/family/groups and other...
    addField(Table_ALERT_RELATED, ALERT_RELATED_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_RELATED, ALERT_RELATED_REL_ID, "RID", FieldIsInteger);
    addField(Table_ALERT_RELATED, ALERT_RELATED_RELATED_TO, "TO", FieldIsInteger);
    addField(Table_ALERT_RELATED, ALERT_RELATED_RELATED_UID, "UID", FieldIsUUID);
    addIndex(Table_ALERT_RELATED, ALERT_RELATED_ID);
    addIndex(Table_ALERT_RELATED, ALERT_RELATED_REL_ID);

    // Translatable description items
    addField(Table_ALERT_LABELS, ALERT_LABELS_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_LABELS, ALERT_LABELS_LABELID, "LID", FieldIsInteger);
    addField(Table_ALERT_LABELS, ALERT_LABELS_LANG, "LNG", FieldIsLanguageText);
    addField(Table_ALERT_LABELS, ALERT_LABELS_VALUE, "VAL", FieldIsShortText);
    addField(Table_ALERT_LABELS, ALERT_LABELS_ISVALID, "ISV", FieldIsBoolean, "1");
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_ID);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_LABELID);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_LANG);

    // Timing
    addField(Table_ALERT_TIMING, ALERT_TIMING_TIMINGID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_TIMING, ALERT_TIMING_TIM_ID, "TIM_ID", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_ISVALID, "ISV", FieldIsBoolean, "1");
    addField(Table_ALERT_TIMING, ALERT_TIMING_STARTDATETIME, "STR", FieldIsDateTime);
    addField(Table_ALERT_TIMING, ALERT_TIMING_ENDDATETIME, "END", FieldIsDateTime);
    addField(Table_ALERT_TIMING, ALERT_TIMING_CYCLES, "CYC", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_CYCLINGDELAY, "CDY", FieldIsUnsignedLongInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_NEXTCYCLE, "NCY", FieldIsDateTime);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_TIMINGID);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_TIM_ID);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_STARTDATETIME);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_ENDDATETIME);

    // Scripting
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_SID, "SID", FieldIsInteger);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_UID, "S_UID", FieldIsUUID);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_ISVALID, "ISV", FieldIsBoolean, "1");
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_TYPE, "S_TP", FieldIsShortText);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_CONTENT, "S_CT", FieldIsLongText);
    addIndex(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_ID);
    addIndex(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_SID);
    addIndex(Table_ALERT_SCRIPTS, ALERT_SCRIPT_UID);

    // Validations
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_VID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_VAL_ID, "VID", FieldIsInteger);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_DATEOFVALIDATION, "DT", FieldIsDateTime);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_VALIDATOR_UUID, "U_U", FieldIsUUID);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_OVERRIDDEN, "OVR", FieldIsBoolean);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_USER_COMMENT, "U_C", FieldIsLongText);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_VALIDATED_UUID, "U_P", FieldIsUUID);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_VID);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_VAL_ID);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_VALIDATOR_UUID);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_VALIDATED_UUID);

    // Packs
    addField(Table_ALERT_PACKS, ALERT_PACKS_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_PACKS, ALERT_PACKS_UID, "UID", FieldIsUUID);
    addField(Table_ALERT_PACKS, ALERT_PACKS_ISVALID, "ISV", FieldIsBoolean);
    addField(Table_ALERT_PACKS, ALERT_PACKS_IN_USE, "INU", FieldIsBoolean);
    addField(Table_ALERT_PACKS, ALERT_PACKS_LABEL_LID, "LBL_LID", FieldIsInteger);
    addField(Table_ALERT_PACKS, ALERT_PACKS_CATEGORY_LID, "CAT_LID", FieldIsInteger);
    addField(Table_ALERT_PACKS, ALERT_PACKS_DESCRIPTION_LID, "DESC_LID", FieldIsInteger);
    addField(Table_ALERT_PACKS, ALERT_PACKS_AUTHORS, "ATH", FieldIsShortText);
    addField(Table_ALERT_PACKS, ALERT_PACKS_VENDOR, "VEN", FieldIsShortText);
    addField(Table_ALERT_PACKS, ALERT_PACKS_URL, "URL", FieldIsShortText);
    addField(Table_ALERT_PACKS, ALERT_PACKS_THEMEDICON, "ICO", FieldIsShortText);
    addField(Table_ALERT_PACKS, ALERT_PACKS_VERSION, "VER", FieldIsShortText);
    addField(Table_ALERT_PACKS, ALERT_PACKS_FMFVERSION, "FMFV", FieldIsShortText);
    addField(Table_ALERT_PACKS, ALERT_PACKS_CREATEDATE, "CDT", FieldIsDateTime);
    addField(Table_ALERT_PACKS, ALERT_PACKS_LASTUPDATE, "UDT", FieldIsDateTime);
    addField(Table_ALERT_PACKS, ALERT_PACKS_XTRAXML, "XTR", FieldIsBlob);

    addIndex(Table_ALERT_PACKS, ALERT_PACKS_UID);

    addField(Table_ALERT_VERSION, VERSION_TEXT, "TXT", FieldIsShortText);

    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

AlertBase::~AlertBase()
{
}

/** Connect to the alert database. Return the connection state. */
bool AlertBase::initialize()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

//    d->checkDatabaseVersion();

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    m_initialized = true;
    return true;
}

/** Private part of the Patients::PatientBase that creates the database. \sa Utils::Database. */
bool AlertBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    Q_UNUSED(access);
    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase dbcreate = QSqlDatabase::addDatabase("QMYSQL", "__ALERTS_CREATOR");
            dbcreate.setHostName(pathOrHostName);
            dbcreate.setUserName(login);
            dbcreate.setPassword(pass);
            dbcreate.setPort(port);
            if (!dbcreate.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), dbcreate);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__ALERTS_CREATOR"))
            QSqlDatabase::removeDatabase("__ALERTS_CREATOR");
        if (!DB.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(DB.connectionName()).arg(DB.lastError().text()),
                                     tkTr(Trans::Constants::CONTACT_DEV_TEAM));
            return false;
        }
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }

    // inform the version
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_ALERT_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }

    return true;
}

/** Create a virtual item. For debugging purpose. */
AlertItem AlertBase::createVirtualItem()
{
    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));

    AlertItem item;
    item.setValidity(true);
    item.setUuid(createUid());
    if (r.randomBool())
        item.setCryptedPassword(r.randomWords(1).toUtf8().toBase64());

    // fr, de, en, xx
    QStringList langs;
    langs << "en" << "fr" << "de" << "xx";
    foreach(const QString &l, langs) {
        item.setLabel(r.randomWords(r.randomInt(2, 10)), l);
        item.setCategory(r.randomWords(r.randomInt(2, 10)), l);
        item.setDescription(r.randomWords(r.randomInt(2, 10)), l);
        item.setComment(r.randomWords(r.randomInt(2, 10)), l);
    }

    item.setViewType(AlertItem::ViewType(r.randomInt(0, AlertItem::NonBlockingAlert)));
    item.setContentType(AlertItem::ContentType(r.randomInt(0, AlertItem::UserNotification)));
    item.setPriority(AlertItem::Priority(r.randomInt(0, AlertItem::Low)));
    item.setCreationDate(r.randomDateTime(QDateTime::currentDateTime()));
    if (r.randomBool())
        item.setLastUpdate(r.randomDateTime(item.creationDate()));
    item.setThemedIcon(r.randomFile(pix, QStringList() << "*.png").fileName());
    if (r.randomBool())
        item.setStyleSheet(r.randomWords(10));
    if (r.randomBool())
        item.setExtraXml(QString("<xml>%1</xml>").arg(r.randomWords(r.randomInt(0, r.randomInt(2, 20)))));

    // Add 1 relation
    AlertRelation rel;
    rel.setRelatedTo(AlertRelation::RelatedToAllPatients);
    item.addRelation(rel);

    // Add timing
    AlertTiming time;
    time.setValid(true);
    time.setStart(r.randomDateTime(QDateTime::currentDateTime()));
    time.setEnd(time.start().addDays(r.randomInt(10, 5000)));
    if (r.randomBool()) {
        time.setCycling(true);
        time.setCyclingDelayInMinutes(r.randomInt(10*24*60, 1000*24*60));
        time.setNumberOfCycles(r.randomInt(1, 100));
    }
    item.addTiming(time);

    // TODO : Add random script
//    item.addScript();

    // TODO : Add random validation
//    item.addValidation();

    item.setModified(false);
    return item;
}

/** Save or update the Alert::AlertItem in the alert database. Return true in case of success. The AlertItem is modified during this process. */
bool AlertBase::saveAlertItem(AlertItem &item)
{
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // update or save ?
    if (!item.db(ItemId).isValid()) {
        // try to catch the id using the uuid
        QHash<int, QString> where;
        where.insert(Constants::ALERT_UID, QString("='%1'").arg(item.uuid()));
        QString req = select(Constants::Table_ALERT, Constants::ALERT_ID, where);
        QSqlQuery query(database());
        if (query.exec(req)) {
            if (query.next())
                item.setDb(ItemId, query.value(0).toInt());
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    if (item.db(ItemId).isValid())
        return updateAlertItem(item);

    database().transaction();
    if (!saveItemRelations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemScripts(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemTimings(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemValidations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemLabels(item)) {
        database().rollback();
        return false;
    }
    if (item.uuid().isEmpty())
        item.setUuid(Database::createUid());
    QSqlQuery query(database());
    QString req = prepareInsertQuery(Constants::Table_ALERT);
    query.prepare(req);
    query.bindValue(Constants::ALERT_ID, QVariant());
    query.bindValue(Constants::ALERT_UID, item.uuid());
    query.bindValue(Constants::ALERT_PACKUID, item.packUid());
    query.bindValue(Constants::ALERT_CATEGORY_UID, item.db(CategoryUid));
    query.bindValue(Constants::ALERT_REL_ID, item.db(RelatedId));
    query.bindValue(Constants::ALERT_SID, item.db(ScriptId));
    query.bindValue(Constants::ALERT_VAL_ID, item.db(ValidationId));
    query.bindValue(Constants::ALERT_TIM_ID, item.db(TimingId));
    query.bindValue(Constants::ALERT_ISVALID, int(item.isValid()));
    query.bindValue(Constants::ALERT_ISREMINDABLE, int(item.isRemindLaterAllowed()));
    query.bindValue(Constants::ALERT_VIEW_TYPE, item.viewType());
    query.bindValue(Constants::ALERT_CONTENT_TYPE, item.contentType());
    query.bindValue(Constants::ALERT_CONDITION_TYPE, QVariant());
    query.bindValue(Constants::ALERT_PRIORITY, item.priority());
    query.bindValue(Constants::ALERT_OVERRIDEREQUIREUSERCOMMENT, int(item.isOverrideRequiresUserComment()));
    query.bindValue(Constants::ALERT_MUSTBEREAD, int(item.mustBeRead()));
    query.bindValue(Constants::ALERT_LABEL_LID, item.db(LabelLID));
    query.bindValue(Constants::ALERT_CATEGORY_LID, item.db(CategoryLID));
    query.bindValue(Constants::ALERT_DESCRIPTION_LID, item.db(DescrLID));
    query.bindValue(Constants::ALERT_COMMENT_LID, item.db(CommentLID));
    query.bindValue(Constants::ALERT_CREATION_DATE, item.creationDate());
    query.bindValue(Constants::ALERT_LAST_UPDATE_DATE, item.lastUpdate());
    query.bindValue(Constants::ALERT_THEMED_ICON, item.themedIcon());
    query.bindValue(Constants::ALERT_THEME_CSS, item.styleSheet());
    query.bindValue(Constants::ALERT_CRYPTED_PASSWORD, item.cryptedPassword());
    query.bindValue(Constants::ALERT_EXTRA_XML, item.extraXml());
    if (query.exec()) {
        item.setDb(ItemId, query.lastInsertId());
        item.setModified(false);
    } else {
        LOG_QUERY_ERROR(query);
        database().rollback();
        return false;
    }
    query.finish();
    database().commit();
    return true;
}

bool AlertBase::updateAlertItem(AlertItem &item)
{
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    if (!item.db(ItemId).isValid())
        return false;

    database().transaction();

    if (!saveItemRelations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemScripts(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemTimings(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemValidations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemLabels(item)) {
        database().rollback();
        return false;
    }

    if (item.uuid().isEmpty())
        item.setUuid(Database::createUid());
    QSqlQuery query(database());
    QList<int> fields;
    fields
            << Constants::ALERT_UID
            << Constants::ALERT_PACKUID
            << Constants::ALERT_CATEGORY_UID
            << Constants::ALERT_REL_ID
            << Constants::ALERT_SID
            << Constants::ALERT_VAL_ID
            << Constants::ALERT_TIM_ID
            << Constants::ALERT_ISVALID
            << Constants::ALERT_ISREMINDABLE
            << Constants::ALERT_VIEW_TYPE
            << Constants::ALERT_CONTENT_TYPE
            << Constants::ALERT_CONDITION_TYPE
            << Constants::ALERT_PRIORITY
            << Constants::ALERT_OVERRIDEREQUIREUSERCOMMENT
            << Constants::ALERT_MUSTBEREAD
            << Constants::ALERT_LABEL_LID
            << Constants::ALERT_CATEGORY_LID
            << Constants::ALERT_DESCRIPTION_LID
            << Constants::ALERT_COMMENT_LID
            << Constants::ALERT_CREATION_DATE
            << Constants::ALERT_LAST_UPDATE_DATE
            << Constants::ALERT_THEMED_ICON
            << Constants::ALERT_THEME_CSS
            << Constants::ALERT_CRYPTED_PASSWORD
            << Constants::ALERT_EXTRA_XML
               ;

    QHash<int, QString> where;
    where.insert(Constants::ALERT_ID, QString("=%1").arg(item.db(ItemId).toString()));
    QString req = prepareUpdateQuery(Constants::Table_ALERT, fields, where);
    query.prepare(req);
    int i = 0;
    query.bindValue(i, item.uuid());
    query.bindValue(++i, item.packUid());
    query.bindValue(++i, item.db(CategoryUid));
    query.bindValue(++i, item.db(RelatedId));
    query.bindValue(++i, item.db(ScriptId));
    query.bindValue(++i, item.db(ValidationId));
    query.bindValue(++i, item.db(TimingId));
    query.bindValue(++i, int(item.isValid()));
    query.bindValue(++i, int(item.isRemindLaterAllowed()));
    query.bindValue(++i, item.viewType());
    query.bindValue(++i, item.contentType());
    query.bindValue(++i, QVariant());
    query.bindValue(++i, item.priority());
    query.bindValue(++i, int(item.isOverrideRequiresUserComment()));
    query.bindValue(++i, int(item.mustBeRead()));
    query.bindValue(++i, item.db(LabelLID));
    query.bindValue(++i, item.db(CategoryLID));
    query.bindValue(++i, item.db(DescrLID));
    query.bindValue(++i, item.db(CommentLID));
    query.bindValue(++i, item.creationDate());
    query.bindValue(++i, item.lastUpdate());
    query.bindValue(++i, item.themedIcon());
    query.bindValue(++i, item.styleSheet());
    query.bindValue(++i, item.cryptedPassword());
    query.bindValue(++i, item.extraXml());
    if (query.exec()) {
        item.setModified(false);
    } else {
        LOG_QUERY_ERROR(query);
        database().rollback();
        return false;
    }
    query.finish();
    database().commit();
    return true;
}

bool AlertBase::saveItemRelations(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // get the related REL_ID
    if (item.relations().count()==0)
        return true;
    int id = -1;
    if (item.db(RelatedId).isValid()) {
        id = item.db(RelatedId).toInt();
    } else {
        id = max(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_REL_ID).toInt() + 1;
        item.setDb(RelatedId, id);
    }
    // save all relations
    QSqlQuery query(database());
    for(int i=0; i<item.relations().count(); ++i) {
        AlertRelation &rel = item.relationAt(i);
        if (rel.id()==-1) {
            // save
            QString req = prepareInsertQuery(Constants::Table_ALERT_RELATED);
            query.prepare(req);
            query.bindValue(Constants::ALERT_RELATED_ID, QVariant());
            query.bindValue(Constants::ALERT_RELATED_REL_ID, id);
            query.bindValue(Constants::ALERT_RELATED_RELATED_TO, rel.relatedTo());
            query.bindValue(Constants::ALERT_RELATED_RELATED_UID, rel.relatedToUid());
            if (query.exec()) {
                rel.setId(query.lastInsertId().toInt());
            } else {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        } else {
            // update
            if (rel.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::ALERT_RELATED_ID, QString("=%1").arg(rel.id()));
                QString req = prepareUpdateQuery(Constants::Table_ALERT_RELATED, QList<int>()
                                                 << Constants::ALERT_RELATED_RELATED_TO
                                                 << Constants::ALERT_RELATED_RELATED_UID,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, rel.relatedTo());
                query.bindValue(++i, rel.relatedToUid());
                if (query.exec()) {
                    rel.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    return false;
                }
            }
        }
    }
    return true;
}

bool AlertBase::saveItemScripts(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    if (item.scripts().count()==0)
        return true;
    // get the script script_id
    int id = -1;
    if (item.db(ScriptId).isValid()) {
        id = item.db(ScriptId).toInt();
    } else {
        id = max(Constants::Table_ALERT_SCRIPTS, Constants::ALERT_SCRIPTS_SID).toInt() + 1;
        item.setDb(ScriptId, id);
    }
    QSqlQuery query(database());
    for(int i=0; i<item.scripts().count(); ++i) {
        AlertScript &script = item.scriptAt(i);
        if (script.id()==-1) {
            // save
            QString req = prepareInsertQuery(Constants::Table_ALERT_SCRIPTS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_SCRIPTS_ID, QVariant());
            query.bindValue(Constants::ALERT_SCRIPTS_SID, id);
            query.bindValue(Constants::ALERT_SCRIPT_UID, script.uuid());
            query.bindValue(Constants::ALERT_SCRIPT_ISVALID, int(script.isValid()));
            query.bindValue(Constants::ALERT_SCRIPT_TYPE, script.type());
            query.bindValue(Constants::ALERT_SCRIPT_CONTENT, script.script());
            if (query.exec()) {
                script.setId(query.lastInsertId().toInt());
            } else {
                LOG_QUERY_ERROR(query);
                return false;
            }
        } else {
            // update
            if (script.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::ALERT_SCRIPTS_ID, QString("=%1").arg(script.id()));
                QString req = prepareUpdateQuery(Constants::Table_ALERT_SCRIPTS, QList<int>()
                                                 << Constants::ALERT_SCRIPT_UID
                                                 << Constants::ALERT_SCRIPT_ISVALID
                                                 << Constants::ALERT_SCRIPT_TYPE
                                                 << Constants::ALERT_SCRIPT_CONTENT,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, script.uuid());
                query.bindValue(++i, int(script.isValid()));
                query.bindValue(++i, script.type());
                query.bindValue(++i, script.script());
                if (query.exec()) {
                    script.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    return false;
                }
            }
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemTimings(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    if (item.timings().count()==0)
        return true;
    // get the timind timing_id
    int id = -1;
    if (item.db(TimingId).isValid()) {
        id = item.db(TimingId).toInt();
    } else {
        id = max(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_TIM_ID).toInt() + 1;
        item.setDb(TimingId, id);
    }
    // save all timings
    QSqlQuery query(database());
    for(int i=0; i<item.timings().count(); ++i) {
        AlertTiming &timing = item.timingAt(i);
        if (timing.id() == -1) {
            // save timing
            QString req = prepareInsertQuery(Constants::Table_ALERT_TIMING);
            query.prepare(req);
            query.bindValue(Constants::ALERT_TIMING_TIMINGID, QVariant());
            query.bindValue(Constants::ALERT_TIMING_TIM_ID, id);
            query.bindValue(Constants::ALERT_TIMING_ISVALID, int(timing.isValid()));
            query.bindValue(Constants::ALERT_TIMING_STARTDATETIME, timing.start());
            if (timing.end().isValid())
                query.bindValue(Constants::ALERT_TIMING_ENDDATETIME, timing.end());
            else
                query.bindValue(Constants::ALERT_TIMING_ENDDATETIME, QString());
            query.bindValue(Constants::ALERT_TIMING_CYCLES, timing.numberOfCycles());
            query.bindValue(Constants::ALERT_TIMING_CYCLINGDELAY, timing.cyclingDelayInMinutes());
            query.bindValue(Constants::ALERT_TIMING_NEXTCYCLE, timing.nextDate());
            if (query.exec()) {
                timing.setId(query.lastInsertId().toInt());
                timing.setModified(false);
            } else {
                LOG_QUERY_ERROR(query);
                return false;
            }
        } else {
            // update timing
            if (timing.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::ALERT_TIMING_TIMINGID, QString("=%1").arg(timing.id()));
                QString req = prepareUpdateQuery(Constants::Table_ALERT_TIMING, QList<int>()
                                                 << Constants::ALERT_TIMING_ISVALID
                                                 << Constants::ALERT_TIMING_STARTDATETIME
                                                 << Constants::ALERT_TIMING_ENDDATETIME
                                                 << Constants::ALERT_TIMING_CYCLES
                                                 << Constants::ALERT_TIMING_CYCLINGDELAY
                                                 << Constants::ALERT_TIMING_NEXTCYCLE,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, int(timing.isValid()));
                query.bindValue(++i, timing.start());
                if (timing.end().isValid())
                    query.bindValue(++i, timing.end());
                else
                    query.bindValue(++i, QString());
                query.bindValue(++i, timing.numberOfCycles());
                query.bindValue(++i, timing.cyclingDelayInMinutes());
                query.bindValue(++i, timing.nextDate());
                if (query.exec()) {
                    timing.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    return false;
                }
            }
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemValidations(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    if (item.validations().count()==0)
        return true;
    // get the validations val_id
    int id = -1;
    if (!item.db(ValidationId).isValid()) {
        id = max(Constants::Table_ALERT_VALIDATION, Constants::ALERT_VALIDATION_VAL_ID).toInt() + 1;
        item.setDb(ValidationId, id);
    } else {
        id = item.db(ValidationId).toInt();
    }
    QSqlQuery query(database());
    for(int i=0; i < item.validations().count(); ++i) {
        AlertValidation &validation = item.validationAt(i);
        if (validation.id() == -1) {
            // save validation
            QString req = prepareInsertQuery(Constants::Table_ALERT_VALIDATION);
            query.prepare(req);
            query.bindValue(Constants::ALERT_VALIDATION_VID, QVariant());
            query.bindValue(Constants::ALERT_VALIDATION_VAL_ID, id);
            query.bindValue(Constants::ALERT_VALIDATION_DATEOFVALIDATION, validation.dateOfValidation());
            query.bindValue(Constants::ALERT_VALIDATION_VALIDATOR_UUID, validation.validatorUid());
            query.bindValue(Constants::ALERT_VALIDATION_OVERRIDDEN, int(validation.isOverriden()));
            query.bindValue(Constants::ALERT_VALIDATION_USER_COMMENT, validation.userComment());
            query.bindValue(Constants::ALERT_VALIDATION_VALIDATED_UUID, validation.validatedUid());
            if (query.exec()) {
                validation.setId(query.lastInsertId().toInt());
                validation.setModified(false);
            } else {
                LOG_QUERY_ERROR(query);
                return false;
            }
        } else {
            // update validation
            if (validation.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::ALERT_VALIDATION_VID, QString("=%1").arg(validation.id()));
                QString req = prepareUpdateQuery(Constants::Table_ALERT_VALIDATION, QList<int>()
                                                 << Constants::ALERT_VALIDATION_DATEOFVALIDATION
                                                 << Constants::ALERT_VALIDATION_VALIDATOR_UUID
                                                 << Constants::ALERT_VALIDATION_OVERRIDDEN
                                                 << Constants::ALERT_VALIDATION_USER_COMMENT
                                                 << Constants::ALERT_VALIDATION_VALIDATED_UUID,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, validation.dateOfValidation());
                query.bindValue(++i, validation.validatorUid());
                query.bindValue(++i, int(validation.isOverriden()));
                query.bindValue(++i, validation.userComment());
                query.bindValue(++i, validation.validatedUid());
                if (query.exec()) {
                    validation.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    return false;
                }
            }
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemLabels(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    QSqlQuery query(database());
    QList<int> lids;
    QList<int> vals;
    const int LABEL = 0;
    const int CATEGORY = 1;
    const int DESCR = 2;
    const int COMMENT = 3;
    // get the labels lid for label, descr && comment
    lids << -1 << -1 << -1 << -1;
    vals << LabelLID << CategoryLID << DescrLID << CommentLID;
    int lastLid = -1;
    for(int i=0; i < vals.count(); ++i) {
        if (item.db(vals.at(i)).isValid() && item.db(vals.at(i)).toInt()>-1) {
            lids[i] = item.db(vals.at(i)).toInt();
            // delete all old relations
            QHash<int, QString> where;
            where.insert(Constants::ALERT_LABELS_LABELID, QString("=%1").arg(lids[i]));
            QString req = prepareDeleteQuery(Constants::Table_ALERT_LABELS, where);
            if (!query.exec(req)) {
                LOG_QUERY_ERROR(query);
                return false;
            }
        } else {
            lids[i] = qMax(lastLid, max(Constants::Table_ALERT_LABELS, Constants::ALERT_LABELS_LABELID).toInt()) + 1;
            item.setDb(vals.at(i), lids.at(i));
        }
        lastLid = lids[i];
        query.finish();
    }
    // save all labels
    foreach(const QString &l, item.availableLanguages()) {
        const QString &label = item.label(l);
        if (!label.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[LABEL]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, label);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
        const QString &cat = item.category(l);
        if (!cat.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[CATEGORY]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, cat);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
        const QString &descr = item.description(l);
        if (!descr.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[DESCR]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, descr);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
        const QString &comment = item.comment(l);
        if (!comment.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[COMMENT]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, comment);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
    }
    return true;
}

/** Return the Alert::AlertItem corresponding to the Alert::Internal::AlertBaseQuery \e query */
QVector<AlertItem> AlertBase::getAlertItems(const AlertBaseQuery &query)
{
    QVector<AlertItem> alerts;
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return alerts;

    // get unique alert by uuid
    if (!query.alertItemFromUuid().isEmpty()) {
        AlertItem item = getAlertItemFromUuid(query.alertItemFromUuid());
        alerts.append(item);
        return alerts;
    }

    // create the where clause according to the query
    Utils::FieldList conds;
    Utils::JoinList joins;
    Utils::Join relatedJoin, timingJoin, validationJoin;

    // All where clauses
    QString wValid;         // ALERT.ISVALID = 1
    QString wTimeValidity;  // basical time validity of alerts
    QString wCycleValidity; // cycling time validity of alerts
    QString wNullValRelated;
    QString wExcludeValidatedUids;   // related to all patient/user but not including the current uid
    QString wUids;          // queried uids (user, patient, application)

    // validity
    Utils::Field valid(Constants::Table_ALERT, Constants::ALERT_ISVALID, QString("=1"));
    wValid = getWhereClause(valid);

    switch (query.alertValidity()) {
    case AlertBaseQuery::ValidAlerts:
    {
        // 1. Manage timings
        //    add join
        timingJoin = Utils::Join(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_TIM_ID, Constants::Table_ALERT, Constants::ALERT_TIM_ID);

        //    add conditions
        //     ------------s--------------e---------------  SQL
        //     ------------vvvvvvvXXvvvvvv----------------  (SQL start date <= alert start date &&  SQL end date > alert start date)
        //    start date > dateRangeStart
        QDateTime start;
        if (!query.dateRangeStart().isValid() || query.dateRangeStart().isNull())
            start = QDateTime(QDate::currentDate(), QTime(0,0,0));
        else
            start = QDateTime(query.dateRangeStart(), QTime(0,0,0));
        conds << Utils::Field(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_STARTDATETIME, QString("<= '%1'").arg(start.toString(Qt::ISODate)));
        conds << Utils::Field(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_ENDDATETIME, QString("> '%1'").arg(start.toString(Qt::ISODate)));

        //     start date <= dateRangeEnd
        if (query.dateRangeEnd().isValid() && !query.dateRangeEnd().isNull()) {
            QDateTime dt = QDateTime(query.dateRangeEnd(), QTime(23,59,59));
            conds << Utils::Field(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_STARTDATETIME, QString("<= '%1'").arg(dt.toString(Qt::ISODate)));
        }
        wTimeValidity += QString("\n       %1").arg(getWhereClause(conds).replace("AND", "\n       AND"));

        // Cycle validity -> get all cycling alerts (date validity is checked by script in the Core)
        conds.clear();
        conds << Utils::Field(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_CYCLES, QString(">0"));
        wTimeValidity += QString("\n       OR %1").arg(getWhereClause(conds));
        wTimeValidity = QString("\n AND (%1\n"
                                 "      )").arg(wTimeValidity);
        conds.clear();

        // 2. User validations
//        conds << Utils::Field(Constants::Table_ALERT, Constants::ALERT_VAL_ID, "IS NULL");
//        wNullValidity += QString("\n AND %1 ").arg(getWhereClause(conds));
        conds.clear();

        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_TO, QString("=%1").arg(AlertRelation::RelatedToAllPatients));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_TO, QString("=%1").arg(AlertRelation::RelatedToAllUsers));
//        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_TO, QString("=%1").arg(AlertRelation::RelatedToApplication));
        wNullValRelated = getWhereClause(conds, Utils::Database::OR);

        relatedJoin = Utils::Join(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_REL_ID, Constants::Table_ALERT, Constants::ALERT_REL_ID);
        validationJoin = Utils::Join(Constants::Table_ALERT_VALIDATION, Constants::ALERT_VALIDATION_VAL_ID, Constants::Table_ALERT, Constants::ALERT_VAL_ID);
        Utils::FieldList excludeValidatedUids;
//        if (patient()) {
            excludeValidatedUids << Utils::Field(Constants::Table_ALERT_VALIDATION, Constants::ALERT_VALIDATION_VALIDATED_UUID, QString("='%1'").arg("patient1"));//patient()->uuid()));
//        }
//        if (user()) {
            excludeValidatedUids << Utils::Field(Constants::Table_ALERT_VALIDATION, Constants::ALERT_VALIDATION_VALIDATED_UUID, QString("='%1'").arg("user1"));//user()->uuid()));
//        }
        if (!excludeValidatedUids.isEmpty()) {
            wExcludeValidatedUids += QString("\n AND %1\n").arg(getWhereClause(excludeValidatedUids, Utils::Database::OR));
        }
        conds.clear();
        break;
    }
    case AlertBaseQuery::InvalidAlerts:
    {
        // add join
        // add conditions
        break;
    }
    case AlertBaseQuery::ValidAndInvalidAlerts:
    {
        // add join
        // add conditions
        break;
    }
    }

    // users
    if (!query.userUids().isEmpty()) {
        conds.clear();
        relatedJoin = Utils::Join(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_REL_ID, Constants::Table_ALERT, Constants::ALERT_REL_ID);
        QString w = QString("IN ('%1')").arg(query.userUids().join("','"));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_TO, QString("=%1").arg(AlertRelation::RelatedToUser));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_UID, w);
        wUids = getWhereClause(conds);
    }
    // patients
    if (!query.patientUids().isEmpty()) {
        conds.clear();
        relatedJoin = Utils::Join(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_REL_ID, Constants::Table_ALERT, Constants::ALERT_REL_ID);
        QString w = QString("IN ('%1')").arg(query.patientUids().join("','"));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_TO, QString("=%1").arg(AlertRelation::RelatedToPatient));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_UID, w);
        if (wUids.isEmpty())
            wUids = getWhereClause(conds);
        else
            wUids = QString("%1\n        OR %2").arg(wUids).arg(getWhereClause(conds));
    }
    // application
    if (!query.applicationNames().isEmpty()) {
        conds.clear();
        relatedJoin = Utils::Join(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_REL_ID, Constants::Table_ALERT, Constants::ALERT_REL_ID);
        QString w = QString("IN ('%1')").arg(query.applicationNames().join("','"));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_TO, QString("=%1").arg(AlertRelation::RelatedToApplication));
        conds << Utils::Field(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_RELATED_UID, w);
        if (wUids.isEmpty())
            wUids = getWhereClause(conds);
        else
            wUids = QString("%1\n        OR %2").arg(wUids).arg(getWhereClause(conds));
    }
    conds.clear();

    if (!timingJoin.isNull()) {
        joins << timingJoin;
    }
    if (!relatedJoin.isNull()) {
        joins << relatedJoin;
    }

    // First select (not including VALIDATION table)
    QString req;
    QString where = QString("%1 %2 %3").arg(wValid).arg(wTimeValidity).arg(wCycleValidity);
    if (wUids.isEmpty()) {
        req = QString("%1"
                      "WHERE %2\n"
                      " AND %3").arg(select(Constants::Table_ALERT, joins, conds)).arg(where).arg(wNullValRelated);
    } else {
        req = QString("%1"
                      "WHERE %2\n"
                      " AND ( %3\n"
                      "       OR ( %4 )\n"
                      "     )").arg(select(Constants::Table_ALERT, joins, conds)).arg(where).arg(wUids).arg(wNullValRelated);
    }

    // second select == alert exclusion (validated for required uids)
    if (!validationJoin.isNull()) {
        joins << validationJoin;
        where = QString("%1 %2 %3 %4").arg(wValid).arg(wTimeValidity).arg(wCycleValidity).arg(wExcludeValidatedUids);
        Utils::Field id(Constants::Table_ALERT, Constants::ALERT_ID);
        QString exclude = QString("%1"
                                  "WHERE %2")
                .arg(select(id, joins, conds)).arg(where);
        Utils::Field excludedIds(Constants::Table_ALERT, Constants::ALERT_ID, QString("NOT IN (\n\n%1\n\n)").arg(exclude));
        req += QString("\n AND %1").arg(getWhereClause(excludedIds));
    }

    if (WarnGetAlertQuerySQLCommand)
        qWarning() << req;

    database().transaction();
    QSqlQuery query(database());
    if (query.exec(req)) {
        while (query.next()) {
            AlertItem item;
            item.setDb(ItemId, query.value(Constants::ALERT_ID));
            item.setDb(RelatedId, query.value(Constants::ALERT_REL_ID));
            item.setDb(CategoryUid, query.value(Constants::ALERT_CATEGORY_UID));
            item.setDb(ScriptId, query.value(Constants::ALERT_SID));
            item.setDb(ValidationId, query.value(Constants::ALERT_VAL_ID));
            item.setDb(TimingId, query.value(Constants::ALERT_TIM_ID));
            item.setDb(LabelLID, query.value(Constants::ALERT_LABEL_LID));
            item.setDb(CategoryLID, query.value(Constants::ALERT_CATEGORY_LID));
            item.setDb(DescrLID, query.value(Constants::ALERT_DESCRIPTION_LID));
            item.setDb(CommentLID, query.value(Constants::ALERT_COMMENT_LID));
            item.setUuid(query.value(Constants::ALERT_UID).toString());
            item.setPackUid(query.value(Constants::ALERT_PACKUID).toString());
            item.setValidity(query.value(Constants::ALERT_ISVALID).toBool());
            item.setRemindLaterAllowed(query.value(Constants::ALERT_ISREMINDABLE).toBool());
            item.setCryptedPassword(query.value(Constants::ALERT_CRYPTED_PASSWORD).toString());
            item.setViewType(AlertItem::ViewType(query.value(Constants::ALERT_VIEW_TYPE).toInt()));
            item.setContentType(AlertItem::ContentType(query.value(Constants::ALERT_CONTENT_TYPE).toInt()));
            item.setPriority(AlertItem::Priority(query.value(Constants::ALERT_PRIORITY).toInt()));
            item.setOverrideRequiresUserComment(query.value(Constants::ALERT_OVERRIDEREQUIREUSERCOMMENT).toBool());
            item.setMustBeRead(query.value(Constants::ALERT_MUSTBEREAD).toBool());
            item.setCreationDate(query.value(Constants::ALERT_CREATION_DATE).toDateTime());
            item.setLastUpdate(query.value(Constants::ALERT_LAST_UPDATE_DATE).toDateTime());
            item.setThemedIcon(query.value(Constants::ALERT_THEMED_ICON).toString());
            item.setStyleSheet(query.value(Constants::ALERT_THEME_CSS).toString());
            item.setExtraXml(query.value(Constants::ALERT_EXTRA_XML).toString());
            alerts << item;
        }
    } else {
        LOG_QUERY_ERROR(query);
        database().rollback();
        return alerts;
    }

    for(int i=0; i < alerts.count(); ++i) {
        AlertItem &item = alerts[i];

        if (!getItemRelations(item)) {
            database().rollback();
            return alerts;
        }
        if (!getItemScripts(item)) {
            database().rollback();
            return alerts;
        }
        if (!getItemTimings(item)) {
            database().rollback();
            return alerts;
        }
        if (!getItemValidations(item)) {
            database().rollback();
            return alerts;
        }
        if (!getItemLabels(item)) {
            database().rollback();
            return alerts;
        }
    }
    database().commit();

    return alerts;
}

AlertItem AlertBase::getAlertItemFromUuid(const QString &uuid)
{
    AlertItem item;
    item.setUuid(uuid);
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return item;
    database().transaction();
    QHash<int, QString> where;
    where.insert(Constants::ALERT_UID, QString("='%1'").arg(uuid));
    QSqlQuery query(database());
    if (query.exec(select(Constants::Table_ALERT, where))) {
        if (query.next()) {
            item.setDb(ItemId, query.value(Constants::ALERT_ID));
            item.setDb(RelatedId, query.value(Constants::ALERT_REL_ID));
            item.setDb(CategoryUid, query.value(Constants::ALERT_CATEGORY_UID));
            item.setDb(ScriptId, query.value(Constants::ALERT_SID));
            item.setDb(ValidationId, query.value(Constants::ALERT_VAL_ID));
            item.setDb(TimingId, query.value(Constants::ALERT_TIM_ID));
            item.setDb(LabelLID, query.value(Constants::ALERT_LABEL_LID));
            item.setDb(CategoryLID, query.value(Constants::ALERT_CATEGORY_LID));
            item.setDb(DescrLID, query.value(Constants::ALERT_DESCRIPTION_LID));
            item.setDb(CommentLID, query.value(Constants::ALERT_COMMENT_LID));
            item.setValidity(query.value(Constants::ALERT_ISVALID).toBool());
            item.setRemindLaterAllowed(query.value(Constants::ALERT_ISREMINDABLE).toBool());
            item.setCryptedPassword(query.value(Constants::ALERT_CRYPTED_PASSWORD).toString());
            item.setViewType(AlertItem::ViewType(query.value(Constants::ALERT_VIEW_TYPE).toInt()));
            item.setContentType(AlertItem::ContentType(query.value(Constants::ALERT_CONTENT_TYPE).toInt()));
            item.setPriority(AlertItem::Priority(query.value(Constants::ALERT_PRIORITY).toInt()));
            item.setOverrideRequiresUserComment(query.value(Constants::ALERT_OVERRIDEREQUIREUSERCOMMENT).toBool());
            item.setMustBeRead(query.value(Constants::ALERT_MUSTBEREAD).toBool());
            item.setCreationDate(query.value(Constants::ALERT_CREATION_DATE).toDateTime());
            item.setLastUpdate(query.value(Constants::ALERT_LAST_UPDATE_DATE).toDateTime());
            item.setThemedIcon(query.value(Constants::ALERT_THEMED_ICON).toString());
            item.setStyleSheet(query.value(Constants::ALERT_THEME_CSS).toString());
            item.setExtraXml(query.value(Constants::ALERT_EXTRA_XML).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        database().rollback();
        return item;
    }

    if (!getItemRelations(item)) {
        database().rollback();
        return item;
    }
    if (!getItemScripts(item)) {
        database().rollback();
        return item;
    }
    if (!getItemTimings(item)) {
        database().rollback();
        return item;
    }
    if (!getItemValidations(item)) {
        database().rollback();
        return item;
    }
    if (!getItemLabels(item)) {
        database().rollback();
        return item;
    }
    database().commit();
    return item;
}

bool AlertBase::getItemRelations(AlertItem &item)
{
    // we are inside a transaction opened by getAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    using namespace Alert::Constants;
    QSqlQuery query(database());
    Utils::Field cond(Table_ALERT, ALERT_ID, QString("=%1").arg(item.db(ItemId).toString()));
    Utils::Join join(Table_ALERT_RELATED, ALERT_RELATED_REL_ID, Table_ALERT, ALERT_REL_ID);
    if (query.exec(select(Table_ALERT_RELATED, join, cond))) {
        while (query.next()) {
            AlertRelation rel;
            rel.setId(query.value(ALERT_RELATED_ID).toInt());
            rel.setRelatedTo(AlertRelation::RelatedTo(query.value(ALERT_RELATED_RELATED_TO).toInt()));
            rel.setRelatedToUid(query.value(ALERT_RELATED_RELATED_UID).toString());
            item.addRelation(rel);
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return true;
}

bool AlertBase::getItemScripts(AlertItem &item)
{
    // we are inside a transaction opened by getAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    using namespace Alert::Constants;
    QSqlQuery query(database());
    Utils::Field cond(Table_ALERT, ALERT_ID, QString("=%1").arg(item.db(ItemId).toString()));
    Utils::Join join(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_SID, Table_ALERT, ALERT_SID);
    if (query.exec(select(Table_ALERT_SCRIPTS, join, cond))) {
        while (query.next()) {
            AlertScript scr;
            scr.setId(query.value(ALERT_RELATED_ID).toInt());
            scr.setValid(query.value(ALERT_SCRIPT_ISVALID).toBool());
            scr.setUuid(query.value(ALERT_SCRIPT_UID).toString());
            scr.setType(AlertScript::ScriptType(query.value(ALERT_SCRIPT_TYPE).toInt()));
            scr.setScript(query.value(ALERT_SCRIPT_CONTENT).toString());
            item.addScript(scr);
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return true;
}

bool AlertBase::getItemTimings(AlertItem &item)
{
    // we are inside a transaction opened by getAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    using namespace Alert::Constants;
    QSqlQuery query(database());
    Utils::Field cond(Table_ALERT, ALERT_ID, QString("=%1").arg(item.db(ItemId).toString()));
    Utils::Join join(Table_ALERT_TIMING, ALERT_TIMING_TIM_ID, Table_ALERT, ALERT_TIM_ID);
    if (query.exec(select(Table_ALERT_TIMING, join, cond))) {
        while (query.next()) {
            AlertTiming time;
            time.setId(query.value(ALERT_TIMING_TIMINGID).toInt());
            time.setValid(query.value(ALERT_TIMING_ISVALID).toBool());
            time.setStart(query.value(ALERT_TIMING_STARTDATETIME).toDateTime());
            if (query.value(ALERT_TIMING_ENDDATETIME).toString().length() > 8)
                time.setEnd(query.value(ALERT_TIMING_ENDDATETIME).toDateTime());
            time.setNumberOfCycles(query.value(ALERT_TIMING_CYCLES).toInt());
            time.setCyclingDelayInMinutes(query.value(ALERT_TIMING_CYCLINGDELAY).toLongLong());
            time.setNextDate(query.value(ALERT_TIMING_NEXTCYCLE).toDateTime());
            item.addTiming(time);
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return true;
}
bool AlertBase::getItemValidations(AlertItem &item)
{
    // we are inside a transaction opened by getAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    using namespace Alert::Constants;
    if (item.relations().count() <= 0) {
        LOG_ERROR("No relations to link validations");
        return false;
    }
    QSqlQuery query(database());
    QStringList uids;
    if (user())
        uids << user()->uuid();
    if (patient())
        uids << patient()->uuid();
    uids << qApp->applicationName().toLower();
    Utils::FieldList conds;
    // Get only validations related to the current user, patient && application
    conds << Utils::Field(Table_ALERT, ALERT_ID, QString("=%1").arg(item.db(ItemId).toString()));
    conds << Utils::Field(Table_ALERT, ALERT_VALIDATION_VALIDATED_UUID, QString("IN ('%1')").arg(uids.join("','")));
    Utils::Join join(Table_ALERT_VALIDATION, ALERT_VALIDATION_VAL_ID, Table_ALERT, ALERT_VAL_ID);
    if (query.exec(select(Table_ALERT_VALIDATION, join, conds))) {
        while (query.next()) {
            AlertValidation val;
            val.setId(query.value(ALERT_VALIDATION_VID).toInt());
            val.setValidatorUuid(query.value(ALERT_VALIDATION_VALIDATOR_UUID).toString());
            val.setOverriden(query.value(ALERT_VALIDATION_OVERRIDDEN).toBool());
            val.setUserComment(query.value(ALERT_VALIDATION_USER_COMMENT).toString());
            val.setDateOfValidation(query.value(ALERT_VALIDATION_USER_COMMENT).toDateTime());
            val.setValidatedUuid(query.value(ALERT_VALIDATION_VALIDATED_UUID).toString());
            item.addValidation(val);
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return true;
}
bool AlertBase::getItemLabels(AlertItem &item)
{
    // we are inside a transaction opened by getAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    using namespace Alert::Constants;
    QSqlQuery query(database());
    Utils::Field cond(Table_ALERT, ALERT_ID, QString("=%1").arg(item.db(ItemId).toString()));

    // get label
    Utils::Join join(Table_ALERT_LABELS, ALERT_LABELS_LABELID, Table_ALERT, ALERT_LABEL_LID);
    if (query.exec(select(Table_ALERT_LABELS, join, cond))) {
        while (query.next()) {
            item.setLabel(query.value(ALERT_LABELS_VALUE).toString(), query.value(ALERT_LABELS_LANG).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();

    // get category
    join = Utils::Join(Table_ALERT_LABELS, ALERT_LABELS_LABELID, Table_ALERT, ALERT_CATEGORY_LID);
    if (query.exec(select(Table_ALERT_LABELS, join, cond))) {
        while (query.next()) {
            item.setCategory(query.value(ALERT_LABELS_VALUE).toString(), query.value(ALERT_LABELS_LANG).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();

    // get description
    join = Utils::Join(Table_ALERT_LABELS, ALERT_LABELS_LABELID, Table_ALERT, ALERT_DESCRIPTION_LID);
    if (query.exec(select(Table_ALERT_LABELS, join, cond))) {
        while (query.next()) {
            item.setDescription(query.value(ALERT_LABELS_VALUE).toString(), query.value(ALERT_LABELS_LANG).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();

    // get comment
    join = Utils::Join(Table_ALERT_LABELS, ALERT_LABELS_LABELID, Table_ALERT, ALERT_COMMENT_LID);
    if (query.exec(select(Table_ALERT_LABELS, join, cond))) {
        while (query.next()) {
            item.setComment(query.value(ALERT_LABELS_VALUE).toString(), query.value(ALERT_LABELS_LANG).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();
    return true;
}

/** Reconnect the database when the database server changes. \sa Core::ICore::databaseServerChanged(), Core::ISettings::databaseConnector() */
void AlertBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    initialize();
}

void AlertBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

/** For debugging purpose */
void AlertBase::toTreeWidget(QTreeWidget *tree)
{
    Database::toTreeWidget(tree);
    tree->expandAll();
}


