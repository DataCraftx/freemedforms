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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Patients::PatientCore
 * \brief Core object of the patientsbaseplugin (namespace Patients).
 * This object can be initialized only if a user is correctly logged.
 */

#include "patientcore.h"
#include "patientbase.h"
#include "patientmodelwrapper.h"
#include "patientmodel.h"
#include "patientwidgetmanager.h"
#include "patientbar.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <translationutils/constants.h>

#include <QDir>
#include <QPointer>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace Patients {

PatientCore *PatientCore::_instance = 0;

namespace Internal {
class PatientCorePrivate
{
public:
    PatientCorePrivate(PatientCore *parent) :
        _base(0),
        _patientModelWrapper(0),
        _patientWidgetManager(0),
        _patientBar(0),
        q(parent)
    {
    }
    
    ~PatientCorePrivate()
    {
    }
    
public:
    PatientBase *_base;
    PatientModelWrapper *_patientModelWrapper;
    PatientWidgetManager *_patientWidgetManager;
    PatientBar *_patientBar;
    QList< QPointer<PatientModel> >_patientModels;

private:
    PatientCore *q;
};
}  // namespace Internal
} // end namespace Patients

/*! Constructor of the Patients::PatientCore class */
PatientCore::PatientCore(QObject *parent) :
    QObject(parent),
    d(new PatientCorePrivate(this))
{
    _instance = this;
    setObjectName("PatientCore");
    d->_base = new PatientBase(this);

    // Create the Core::IPatient
    d->_patientModelWrapper = new Internal::PatientModelWrapper(this);
    Core::ICore::instance()->setPatient(d->_patientModelWrapper);
}

/*! Destructor of the Patients::PatientCore class */
PatientCore::~PatientCore()
{
    delete d->_patientBar;
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PatientCore::initialize()
{
    if (!d->_base->initialize())
        return false;

    d->_patientWidgetManager = new PatientWidgetManager(this);

    // create singleton model
    PatientModel *model = new PatientModel(this);
    d->_patientModelWrapper->initialize(model);

    d->_patientBar = new PatientBar;

    return true;
}

/** You must register all your patient models to keep them sync */
void PatientCore::registerPatientModel(PatientModel *model)
{
    d->_patientModels << QPointer<PatientModel>(model);
}

/** Initialization with a full opened Core::ICore */
void PatientCore::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;

//    PatientModel::activeModel()->refreshModel();
    d->_patientWidgetManager->postCoreInitialization();
}

/**
 * Create the default virtual patients: Archer, Kirk, Janeway...
 */
bool PatientCore::createDefaultVirtualPatients() const
{
    QString path = settings()->path(Core::ISettings::BigPixmapPath) + QDir::separator();
    int userLkId = 1;

    QString uid = "b04936fafccb4174a7a6af25dd2bb71c";
    d->_base->createVirtualPatient("KIRK", "", "James Tiberius", "M", 6, QDate(1968, 04, 20), "US", "USS Enterprise",
                  "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainkirk.jpg");

    uid = "2c49299b9b554300b46a6e3ef6d40a65";
    d->_base->createVirtualPatient("PICARD", "", "Jean-Luc", "M", 6, QDate(1948, 04, 20), "US", "USS Enterprise-D",
                  "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainpicard.png");

    uid = "ef97f37361824b6f826d5c9246f9dc49";
    d->_base->createVirtualPatient("ARCHER", "", "Jonathan", "M", 6, QDate(1928, 04, 20), "US", "Enterprise (NX-01) commanding officer",
                  "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainarcher.jpg");

    uid = "493aa06a1b8745b2ae6c79c531ef12a0";
    d->_base->createVirtualPatient("JANEWAY", "", "Kathryn", "F", 6, QDate(1938, 04, 20), "US", "USS Voyager",
                  "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainjaneway.jpg");
    return true;
}

/**
 * Return the unique instance of the Patients::Internal::PatientBase. \n
 * Avoid usage of this method (mainly plugin internal method).
 */
Internal::PatientBase *PatientCore::patientBase() const
{
    return d->_base;
}

/**
 * Return the unique instance of the Patients::Internal::PatientWidgetManager. \n
 * Avoid usage of this method (mainly plugin internal method).
 */
Internal::PatientWidgetManager *PatientCore::patientWidgetManager() const
{
    return d->_patientWidgetManager;
}

/**
 * Return the unique instance of the PatientBar. In this configuration,
 * the PatientBar does not have any parent. The deletion of the pointer
 * is managed by this object.
 */
PatientBar *PatientCore::patientBar() const
{
    return d->_patientBar;
}

/**
 * Define the current patient using its \e uuid. Return true if the current patient
 * was correctly set.
 */
bool PatientCore::setCurrentPatientUuid(const QString &uuid)
{
    // Take the Core:IPatient internal PatientModel
    PatientModel *patientModel = d->_patientModelWrapper->patientModel();

    // Start changing the current patient
    if (!patientModel->beginChangeCurrentPatient())
        return false;

    // Update the filter to the correct uuid
    patientModel->setFilter("", "", uuid, PatientModel::FilterOnUuid);
    if (patientModel->numberOfFilteredPatients() != 1) {
        LOG_ERROR(QString("No patient found; Number of uuids: %1")
                  .arg(patientModel->numberOfFilteredPatients()));
        return false;
    }

    // Define the new current patient
    patientModel->setCurrentPatient(patientModel->index(0,0));

    // Finish the current patient modification process
    patientModel->endChangeCurrentPatient();

    return true;
}

/**
 * Force refreshing of all registered PatientModel. This can be CPU consuming has
 * all the PatientModel will re-select the SQL database.
 * \sa registerPatientModel()
 */
void PatientCore::refreshAllPatientModel() const
{
    d->_patientModels.removeAll(0);
    foreach(PatientModel *model, d->_patientModels)
        model->refreshModel();
}
