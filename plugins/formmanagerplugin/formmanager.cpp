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

/**
   \class Form::FormManager
    This class manages all aspect of the patient's forms.
*/

#include "formmanager.h"
#include "iformitem.h"
#include "iformio.h"
#include "formplaceholder.h"
#include "episodebase.h"
#include "constants_db.h"
#include "subforminsertionpoint.h"

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <patientbaseplugin/constants_menus.h>

#include <utils/global.h>
#include <utils/log.h>

#include <extensionsystem/pluginmanager.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QStackedLayout>
#include <QSplitter>
#include <QWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPointer>
#include <QMap>
#include <QApplication>
#include <QPixmap>

enum { WarnFormCreation = false };

using namespace Form;
using namespace Form::Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
inline static Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}


namespace Form {
namespace Constants {
    const char * const XML_FORM_GENERAL_TAG = "FormXmlContent";
}

namespace Internal {
class FormManagerPrivate
{
public:
    FormManagerPrivate(FormManager *parent) :
            m_ActualEpisode(-1), q(parent)
    {}

    ~FormManagerPrivate()
    {
        qDeleteAll(m_RootForms);
        m_RootForms.clear();
    }

public:
    /** \todo create a EpisodeData class */
    int m_ActualEpisode;
    QString m_ActualEpisode_FormUid;
    QList<Form::FormMain *> m_RootForms, m_SubFormsEmptyRoot;


private:
    FormManager *q;
};
} // End Internal
} // End Form


FormManager *FormManager::m_Instance = 0;

/** Unique instance of the Form::FormManager object */
FormManager *FormManager::instance()
{
    if (!m_Instance) {
        m_Instance = new FormManager(qApp);
    }
    return m_Instance;
}

FormManager::FormManager(QObject *parent) :
        FormActionHandler(parent),
        d(new Form::Internal::FormManagerPrivate(this))
{
    setObjectName("FormManager");

//    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(loadPatientFile()));
}

FormManager::~FormManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/**  Activate the Form Mode in the main window. */
void FormManager::activateMode()
{
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
}

/**  Return all available empty root forms for the current patient. \sa Form::FormManager::subFormsEmptyRoot(), Form::FormMain */
QList<FormMain *> FormManager::forms() const
{
    return d->m_RootForms;
}

/**  Return all available empty root sub-forms for the current patient. \sa Form::FormMain */
QList<FormMain *> FormManager::subFormsEmptyRoot() const
{
    return d->m_SubFormsEmptyRoot;
}

/**
  Return the empty root forms loaded from the \e formUid by the IFormIO objects. \n
  All these forms are stored in the plugin manager object pool and can be accessed using
  forms().
 */
QList<Form::FormMain *> FormManager::loadFormFile(const QString &formUid)
{
    QList<Form::FormMain *> toReturn;

    if (formUid.isEmpty()) {
        LOG_ERROR("No formUid to load...");
        return toReturn;
    }

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    if (list.isEmpty()) {
        LOG_ERROR("No IFormIO loaded...");
        return toReturn;
    }

    // Load root forms
    foreach(Form::IFormIO *io, list) {
        if (io->canReadForms(formUid)) {
            toReturn << io->loadAllRootForms(formUid);
        }
    }

    return toReturn;
}

/** Load the generic patient file (and included subforms) and emit patientFormsLoaded() when finished. */
bool FormManager::loadPatientFile()
{
    qDeleteAll(d->m_RootForms);
    d->m_RootForms.clear();

    // get form general form absPath from episodeBase
    QString absDirPath = episodeBase()->getGenericFormFile();
    if (absDirPath.isEmpty()) {
        /** \todo code here: manage no patient form file recorded in episodebase */
        return false;
    }
    // load central root forms
    d->m_RootForms = loadFormFile(absDirPath);

    // load subforms
    loadSubForms();

    // emit signal
    Q_EMIT patientFormsLoaded();

    return true;
}

/**
  Extract from the <b>patient file form</b> the empty root form corresponding to the \e modeUniqueName
  or 0 if no form matches the \e modeUniqueName.
  \sa loadPatientFile()
*/
Form::FormMain *FormManager::rootForm(const char *modeUniqueName)
{
    // get all root form from the plugin manager
    for(int i=0; i < d->m_RootForms.count(); ++i) {
        FormMain *root = d->m_RootForms.at(i);
        if (root->modeUniqueName().compare(QString(modeUniqueName), Qt::CaseInsensitive)==0) {
            return root;
        }
    }
    return 0;
}

/** Load all requiered subforms and insert them according to data extracted from the episode database. */
bool FormManager::loadSubForms()
{
    d->m_SubFormsEmptyRoot.clear();
    // get sub-forms from database
    const QVector<SubFormInsertionPoint> &subs = episodeBase()->getSubFormFiles();
    if (subs.isEmpty()) {
        return true;
    }

    bool ok = true;
    for(int i = 0; i < subs.count(); ++i) {
        if (!insertSubForm(subs.at(i)))
            ok = false;
    }

    return ok;
}

/** Insert a sub-form to a form to the specified \e insertionPoint. A signal is emitted before Form::FormMain are getting reparented. */
bool FormManager::insertSubForm(const SubFormInsertionPoint &insertionPoint)
{
    // read all sub-forms and emit signal if requiered
    QList<Form::FormMain*> subs = loadFormFile(insertionPoint.subFormUid());
    d->m_SubFormsEmptyRoot << subs;
    if (insertionPoint.emitInsertionSignal())
        Q_EMIT subFormLoaded(insertionPoint.subFormUid());

    // insert sub-forms to the forms tree
    const QString &insertIntoUuid = insertionPoint.receiverUid();
    for(int i=0; i < subs.count(); ++i) {
        FormMain *sub = subs.at(i);
//        qWarning() << "insert" << sub->uuid() << "to" << insertIntoUuid;
        if (insertIntoUuid == Constants::ROOT_FORM_TAG) {
            // insert into its mode root form
            FormMain *rootMode = rootForm(sub->modeUniqueName().toAscii());
            if (!rootMode) {
                LOG_ERROR("unable to find the mode root form: " + sub->modeUniqueName());
                continue;
            }
            foreach(Form::FormMain *form, sub->firstLevelFormMainChildren())
                form->setParent(rootMode);
        }

        // find point of insertion
        QList<Form::FormMain *> allForms = forms();
        for(int j=0; j < allForms.count(); ++j) {
            FormMain *parent = allForms.at(j);
            // search inside flatten children
            QList<Form::FormMain *> children = parent->flattenFormMainChildren();
            for(int k=0; k < children.count(); ++k) {
                FormMain *child = children.at(k);
                if (child->uuid()==insertIntoUuid) {
//                    qWarning() << "inserting subForm"<< insertionPoint.subFormUid() << "to" << insertionPoint.receiverUid();
                    foreach(Form::FormMain *form, sub->firstLevelFormMainChildren()) {
                        form->setParent(child);
                    }
                    break;
                }
            }
        }
    }
    return true;
}

/** Read the PMHx categories associated the \e formUidOrAbsPath form. */
bool FormManager::readPmhxCategories(const QString &formUuidOrAbsPath)
{
//    qWarning() << Q_FUNC_INFO;
    /** \todo code here why don't we use the \e formUuidOrAbsPath param ??? */
    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();

    // get form general form absPath from episodeBase
    QString absDirPath = episodeBase()->getGenericFormFile();
    if (absDirPath.isEmpty()) {
        /** \todo code here: manage no patient form file recorded in episodebase */
        return false;
    }
    foreach(Form::IFormIO *io, list) {
        if (io->canReadForms(absDirPath)) {
            if (io->loadPmhCategories(absDirPath))
                break;
        }
    }
    return true;
}

/** Return one specific screenshot for the form \e formUid and nammed \e fileName. */
QPixmap FormManager::getScreenshot(const QString &formUid, const QString &fileName)
{
    if (formUid.isEmpty()) {
        LOG_ERROR("No formUid...");
        return QPixmap();
    }

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    if (list.isEmpty()) {
        LOG_ERROR("No IFormIO loaded...");
        return QPixmap();
    }

    // Load root forms
    QPixmap pix;
    foreach(Form::IFormIO *io, list) {
        pix = io->screenShot(formUid, fileName);
        if (!pix.isNull())
            return pix;
    }
    return pix;
}

/** Execute all OnLoad scripts of the \e emptyRootForm */
void FormManager::executeOnloadScript(Form::FormMain *emptyRootForm)
{
    scriptManager()->evaluate(emptyRootForm->scripts()->onLoadScript());
    QList<Form::FormMain *> children = emptyRootForm->flattenFormMainChildren();
    foreach(Form::FormMain *mainChild, children) {
        scriptManager()->evaluate(mainChild->scripts()->onLoadScript());
        foreach(Form::FormItem *item, mainChild->flattenFormItemChildren()) {
            scriptManager()->evaluate(item->scripts()->onLoadScript());
        }
    }
}


FormActionHandler::FormActionHandler(QObject *parent) :
    QObject(parent),
    aAddEpisode(0),
    aValidateEpisode(0),
    aAddForm(0),
    aPrintForm(0),
    aShowPatientLastEpisode(0)
{
    Core::ActionManager *am = actionManager();
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();
    QList<int> formContext = QList<int>() << uid->uniqueIdentifier(Constants::C_FORM_PLUGINS);
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

    // Form's Contextual Menu
//    Core::ActionContainer *editMenu = am->actionContainer(Core::Constants::M_EDIT);
//    Core::ActionContainer *cmenu = am->actionContainer(Core::Constants::M_EDIT_LIST);
//    if (!cmenu) {
//        cmenu = am->createMenu(Core::Constants::M_EDIT_LIST);
//        cmenu->appendGroup(Core::Constants::G_EDIT_LIST);
//        cmenu->setTranslations(Trans::Constants::M_EDIT_LIST_TEXT);
//        if (editMenu)
//            editMenu->addMenu(cmenu, Core::Constants::G_EDIT_LIST);
//    }

    QAction *a = aAddEpisode = new QAction(this);
    a->setObjectName("aAddEpisode");
    a->setIcon(th->icon(Core::Constants::ICONADD));
    Core::Command *cmd = am->registerAction(a, Constants::A_ADDEPISODE, formContext);
    cmd->setTranslations(Constants::ADDEPISODE_TEXT, Constants::ADDEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aValidateEpisode = new QAction(this);
    a->setObjectName("aValidateEpisode");
    a->setIcon(th->icon(Core::Constants::ICONVALIDATEDARK));
    cmd = am->registerAction(a, Constants::A_VALIDATEEPISODE, formContext);
    cmd->setTranslations(Constants::VALIDATEEPISODE_TEXT, Constants::VALIDATEEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aAddForm = new QAction(this);
    a->setObjectName("aAddForm");
    a->setIcon(th->icon(Core::Constants::ICONFORMS));
    cmd = am->registerAction(a, Constants::A_ADDFORM, formContext);
    cmd->setTranslations(Constants::ADDFORM_TEXT, Constants::ADDFORM_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aPrintForm = new QAction(this);
    a->setObjectName("aPrintForm");
    a->setIcon(th->icon(Core::Constants::ICONPRINT));
    cmd = am->registerAction(a, Constants::A_PRINTFORM, formContext);
    cmd->setTranslations(Constants::PRINTFORM_TEXT, Constants::PRINTFORM_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aShowPatientSynthesis = new QAction(this);
    a->setEnabled(false);
    a->setObjectName("aShowPatientSynthesis");
    a->setIcon(th->icon(Core::Constants::ICONPATIENTSYNTHESIS, Core::ITheme::MediumIcon));
    cmd = am->registerAction(a, Constants::A_SHOWPATIENTSYNTHESIS, globalcontext);
    cmd->setTranslations(Constants::SHOWPATIENTSYNTHESIS_TEXT, Constants::SHOWPATIENTSYNTHESIS_TEXT, Constants::FORM_TR_CONTEXT);
    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_PATIENTS);
    if (menu)
        menu->addAction(cmd, Patients::Constants::G_PATIENTS_INFORMATIONS);
//    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(showPatientSynthesis()));

    a = aShowPatientLastEpisode = new QAction(this);
    a->setEnabled(false);
    a->setObjectName("aShowPatientLastEpisode");
    a->setIcon(th->icon(Core::Constants::ICONPATIENTSYNTHESIS, Core::ITheme::MediumIcon));
    cmd = am->registerAction(a, Constants::A_SHOWPATIENTLASTEPISODES, globalcontext);
    cmd->setTranslations(Constants::SHOWPATIENTLASTEPISODES_TEXT, Constants::SHOWPATIENTLASTEPISODES_TEXT, Constants::FORM_TR_CONTEXT);
    if (menu)
        menu->addAction(cmd, Patients::Constants::G_PATIENTS_INFORMATIONS);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(showPatientLastEpisode()));

    contextManager()->updateContext();
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(updateActions()));
}

FormActionHandler::~FormActionHandler()
{
}

void FormActionHandler::showPatientLastEpisode()
{
    FormManager::instance()->activateMode();
}

void FormActionHandler::updateActions()
{
    aShowPatientLastEpisode->setEnabled(true);
    /** \todo remove when patient synthesis will be available */
    aShowPatientSynthesis->setEnabled(false);
}
