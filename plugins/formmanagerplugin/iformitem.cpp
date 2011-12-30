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

/**
 \class Form::FormItem
 This object represents one element of the patient file form.\n
 It contains:
  - one Form::FormItemSpec with description of the item: author, version, licence...
  - one Form::FormItemScripts for the script management
  - one Form::FormItemValues
  - you can set its Form::IFormItemData for the database access
  - you can set its Form::IFormWidget
 If you intend to create a child of this item use the specific members:
  - createChildForm() to create a new Form::FormMain
  - createChildItem() to create a new Form::FormItem
  - createPage() to create a new Form::FormPage
 You can get all its children by calling: formItemChildren().\n
 You can define extra-datas (options) with the addExtraData(), extraDatas() and clearExtraDatas().
 You can define the Core::IPatient representation of this Form::FormItem with setPatientDataRepresentation(),
 patientDataRepresentation().

 \todo - Options d'affichage et d'impression ??
 \todo - Options de "droit" / utilisateurs autorisés
 \todo - Scripts (OnLoad, PostLoad, CallMe)
 \todo - Base de règles
 \todo - Plugin associé
 \todo - 1 QWidget vue (les widgets sont construits par les IFormIO selon leur spécificités)
 \todo - 1 QWidget historique
 \todo - checkValueIntegrity() qui se base sur les règles de l'item pour vérifier son exactitude
*/

/**
  \class Form::FormPage
  Actually unused.
*/

/**
  \class Form::FormMain
  A Form::FormMain represents a root item of a form. Usually, the Form::FormMain are created by the
  Form::IFormIO engines. The first item returned by the Form::IFormIO when loading files are empty and represents
  the root of the form.\n
  When you need to create a FormMain as child to one another use the createChildForm() member. Get all its children
  (including sub-trees) using the flattenFormMainChildren() member or the formMainChild() if you want a specific child.\n
  setEpisodePossibilities() and episodePossibilities() are used in the Form::EpisodeModel when creating the tree model.\n
  When a mode creates a root Form::FormMain it should declare the main empty root object
  it in the plugin manager object pool to allow other object to access the datas of forms (eg: PatientModelWrapper).\n
  \code
  ExtensionSystem::PluginManager::instance()->addObject(myRootFormMain);
  \endcode
*/

/**
  \class Form::FormItemSpec
  \todo Documentation
*/

/**
  \class Form::FormItemScripts
  Stores the scripts associated with the Form::FormItem
  \todo Documentation
*/

/**
  \class Form::FormItemValues
  \todo Documentation
*/

#include "iformitem.h"

#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/global.h>
#include <utils/log.h>

#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QGridLayout>
#include <QHeaderView>
#include <QMap>

#include <QDebug>

using namespace Form;
using namespace Form::Internal;

inline static Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::UniqueIDManager *uuidManager() {return Core::ICore::instance()->uniqueIDManager();}

#ifdef DEBUG
enum {WarnFormCreation=false};
#else
enum {WarnFormCreation=false};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  FormItemIdentifiants  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FormItemIdentifiants::setUuid(const QString &uuid)
{
    id = uuidManager()->uniqueIdentifier(uuid);
    m_Uuid=uuid;

    /** \todo Define the objectName according to the parent to simplify the scripting */
//    QString objName = uuid;
//    if (parent()) {
//        qWarning() << "xxxxxx" << parent()->objectName() << uuid;
//        if (uuid.startsWith(parent()->objectName())) {
//            objName = objName.mid(parent()->objectName().length());
//            while (objName.startsWith(".")) {
//                objName = objName.mid(1);
//            }
//            while (objName.startsWith(":")) {
//                objName = objName.mid(1);
//            }
//        }
//    }
//    setObjectName(objName);
}

QString FormItemIdentifiants::uuid() const
{
    return m_Uuid;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  FormItemScripts  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ScriptsBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l)
    {
        new QTreeWidgetItem(l, QStringList() << "OnLoad" << m_Scripts.value(0) );
        new QTreeWidgetItem(l, QStringList() << "PostLoad" << m_Scripts.value(1) );
        new QTreeWidgetItem(l, QStringList() << "OnDemand" << m_Scripts.value(2) );
        new QTreeWidgetItem(l, QStringList() << "OnValueChanged" << m_Scripts.value(3) );
        new QTreeWidgetItem(l, QStringList() << "OnValueRequiered" << m_Scripts.value(4) );
        new QTreeWidgetItem(l, QStringList() << "OnDependentValueChanged" << m_Scripts.value(5) );
    }

    QHash<int, QString> m_Scripts;
    QHash<int, int> m_States;
};

namespace Form {
namespace Internal {
class FormItemScriptsPrivate : public Trans::MultiLingualClass<ScriptsBook>
{
public:
    FormItemScriptsPrivate() {}
    ~FormItemScriptsPrivate() {}

    QString categoryForTreeWiget() const {return QString("Scripts");}
};

} // End Internal
} // End Form

FormItemScripts::FormItemScripts(
        const QString &lang,
        const QString &onLoad,
        const QString &postLoad,
        const QString &onDemand,
        const QString &onValChanged,
        const QString &onValRequiered,
        const QString &onDependentValuesChanged ) :
        d(new FormItemScriptsPrivate)
{
    ScriptsBook *s = d->createLanguage(lang.left(2));
    s->m_Scripts.insert(Script_OnLoad, onLoad);
    s->m_Scripts.insert(Script_PostLoad, postLoad);
    s->m_Scripts.insert(Script_OnDemand, onDemand);
    s->m_Scripts.insert(Script_OnValueChanged, onValChanged);
    s->m_Scripts.insert(Script_OnValueRequiered, onValRequiered);
    s->m_Scripts.insert(Script_OnDependentValueChanged, onDependentValuesChanged);
}

FormItemScripts::~FormItemScripts()
{
    if (d) delete d;
    d=0;
}

void FormItemScripts::setScript(const int type, const QString &script, const QString &lang)
{
//    WARN_FUNC << type << script << lang;
    ScriptsBook *s = 0;
    if (d->hasLanguage(lang))
        s = d->getLanguage(lang);
    else
        s = d->createLanguage(lang);
    s->m_Scripts.insert(type,script);
}

QString FormItemScripts::script(const int type, const QString &lang) const
{
    ScriptsBook *s = d->getLanguage(lang);
    if (!s) {
        s = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!s) {
            s = d->getLanguage("en");
            if (!s)
                return QString::null;
        }
    }
    return s->m_Scripts.value(type);
}

void FormItemScripts::toTreeWidget(QTreeWidgetItem *tree)
{
    d->toTreeWidget(tree);
}

void FormItemScripts::warn() const
{
    ScriptsBook *s = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
    Utils::quickDebugDialog(
            QStringList()
            << QString("Script_OnLoad\n") + s->m_Scripts.value(Script_OnLoad)
            << QString("Script_PostLoad\n") + s->m_Scripts.value(Script_PostLoad)
            << QString("Script_OnDemand\n") + s->m_Scripts.value(Script_OnDemand)
            << QString("Script_OnValueChanged\n") + s->m_Scripts.value(Script_OnValueChanged)
            << QString("Script_OnValueRequiered\n") + s->m_Scripts.value(Script_OnValueRequiered)
            << QString("Script_OnDependentValueChanged\n") + s->m_Scripts.value(Script_OnDependentValueChanged)
            );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  FormItemValueReferences  ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Form {
namespace Internal {

class ValuesBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l)
    {
        QTreeWidgetItem *i = 0;
        i = new QTreeWidgetItem(l, QStringList() << "Possibles");
        foreach(int id, m_Possible.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Possible.value(id).toString() );
        }

        i = new QTreeWidgetItem(l, QStringList() << "Numerical");
        foreach(int id, m_Numerical.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Numerical.value(id).toString() );
        }

        i = new QTreeWidgetItem(l, QStringList() << "Script");
        foreach(int id, m_Script.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Script.value(id).toString() );
        }
    }

public:
    QMap<int, QVariant> m_Possible;
    QMap<int, QVariant> m_Script;
    QMap<int, QVariant> m_Numerical;
    QMap<int, QVariant> m_Printing;
    QMap<int, QVariant> m_Uuid;
    QVariant m_Default;
};

class FormItemValuesPrivate : public Trans::MultiLingualClass<ValuesBook>
{
public:
    FormItemValuesPrivate() {}
    ~FormItemValuesPrivate() {}

    QString categoryForTreeWiget() const {return QString("Value References");}
public:
    bool m_Optionnal;
};
}  // End Internal
}  // End Form

FormItemValues::FormItemValues(QObject *parent) :
        QObject(parent), d(new Form::Internal::FormItemValuesPrivate)
{
}

FormItemValues::~FormItemValues()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines a value \e val for one \e type at one specified \e id in a virtual list, for one \e language.
  \e language must be a valid local language name in 2 chars encoding.
*/
void FormItemValues::setValue(int type, const int id, const QVariant &val, const QString &language)
{
    QString l = language;
    if (language.isEmpty() || (type==Value_Uuid))
        l = Trans::Constants::ALL_LANGUAGE;
    ValuesBook *values = d->createLanguage(l);
//    qWarning() << "SETVALUE" << l << language << values << type << id << val;
    switch (type)
    {
    case Value_Uuid :      values->m_Uuid.insert(id,val); break;
    case Value_Numerical : values->m_Numerical.insert(id,val); break;
    case Value_Script :    values->m_Script.insert(id,val); break;
    case Value_Possible :  values->m_Possible.insert(id,val); break;
    case Value_Printing:   values->m_Printing.insert(id,val); break;
    case Value_Default :   values->m_Default = val; break;
    case Value_Dependency : break;
    }
}

void FormItemValues::setDefaultValue(const QVariant &val, const QString &lang)
{
    QString l = lang;
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    ValuesBook *values = d->createLanguage(l);
    values->m_Default = val;
}

QVariant FormItemValues::defaultValue(const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty())
        l = QLocale().name();
    ValuesBook *values = d->getLanguage(l);
    QVariant ret;
    if (values)
        ret = values->m_Default;
    if (ret.isNull() && l==QLocale().name()) {
        values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (values)
            return values->m_Default;
    }
    return QVariant();
}

//void FormItemValues::setSelectedValue(const QVariant &val)
//{
//    d->m_SelectedValue = val;
//}
//
//QVariant FormItemValues::selectedValue() const
//{
//    return d->m_SelectedValue;
//}

bool FormItemValues::isOptionnal() const
{
    return d->m_Optionnal;
}

void FormItemValues::setOptionnal(bool state)
{
    d->m_Optionnal = state;
}

/**
  \brief Returns a QStringList of values asked sorted by id number. \e typeOfValue represents the enumerator.
*/
QStringList FormItemValues::values(const int typeOfValues) const
{
    if (typeOfValues==Value_Uuid) {
        // return the ALL_LANGUAGE uuids
        ValuesBook *values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!values)
            return QStringList();
        QStringList list;
        foreach(const QVariant &uuid, values->m_Uuid.values()) {
            list.append(uuid.toString());
        }
        return list;
    } else if (typeOfValues==Value_Default) {
        return QStringList() << defaultValue().toString();
    } else if (typeOfValues==Value_Dependency) {
        return QStringList();
    }
    ValuesBook *values = d->getLanguage(QLocale().name());
    QMap<int, QVariant> map;
    if (values) {
        switch (typeOfValues)
        {
        case Value_Numerical : map = values->m_Numerical; break;
        case Value_Script :    map = values->m_Script; break;
        case Value_Possible :  map = values->m_Possible; break;
        case Value_Printing :  map = values->m_Printing; break;
        }
    }
    if (map.isEmpty()) {
        values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!values) {
            return QStringList();
        }
        switch (typeOfValues)
        {
        case Value_Numerical : map = values->m_Numerical; break;
        case Value_Script :    map = values->m_Script; break;
        case Value_Possible :  map = values->m_Possible; break;
        case Value_Printing :  map = values->m_Printing; break;
        }
    }
    QStringList list;
    foreach(int i, map.keys()) {
        list.append(map.value(i).toString());
    }
    return list;
}


void FormItemValues::toTreeWidget(QTreeWidgetItem *tree) const
{
    d->toTreeWidget(tree);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormItem  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormItem::~FormItem()
{
    if (m_Spec) {
        delete m_Spec;
        m_Spec = 0;
    }
    if (m_Scripts) {
        delete m_Scripts;
        m_Scripts = 0;
    }
    if (m_ItemDatas) {
        delete m_ItemDatas;
        m_ItemDatas = 0;
    }
}

void FormItem::addExtraData(const QString &id, const QString &data)
{
    if (m_ExtraDatas.keys().indexOf(id, Qt::CaseInsensitive) != -1) {
        QString add = m_ExtraDatas.value(id) + ";" + data;
        m_ExtraDatas.insert(id, add);
    } else {
        m_ExtraDatas.insert(id,data);
    }
}

QStringList FormItem::getOptions() const
{
    QStringList l;
    l = m_ExtraDatas.value("options").split(";", QString::SkipEmptyParts);
    l += m_ExtraDatas.value("option").split(";", QString::SkipEmptyParts);
    return l;
}

FormItem *FormItem::createChildItem(const QString &uuid)
{
    Form::FormItem *i = new Form::FormItem(this);
    if (!uuid.isEmpty())
        i->setUuid(uuid);
    return i;
}

void FormItem::languageChanged()
{
    qWarning() << "FormItem language changed" << uuid();
}

void FormPage::languageChanged()
{
    qWarning() << "FormPage language changed" << uuid();
}


//FormPage *FormPage::createPage(const QString &uuid)
//{
//    FormPage *p = new FormPage(this);
//    if (!uuid.isEmpty())
//        p->setUuid(uuid);
//    return p;
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormMain  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormMain::FormMain(QObject *parent) :
    FormItem(parent),
    m_DebugPage(0),
    m_Episode(MultiEpisode),
    m_UseNameAsNSForSubItems(false)
{
}

FormMain::~FormMain()
{
    /** \todo this is buggy */
//    if (m_DebugPage)
//        ExtensionSystem::PluginManager::instance()->removeObject(m_DebugPage);
}

/** \brief Create and return a new FormMain as children of this FormMain. */
FormMain *FormMain::createChildForm(const QString &uuid)
{
    FormMain *f = new FormMain(this);
    if (!uuid.isEmpty())
        f->setUuid(uuid);
    if (WarnFormCreation)
        LOG("FormManager Creating Form: " + uuid + " has child of " + this->uuid());
    return f;
}

/** \brief Actually unused */
FormPage *FormMain::createPage(const QString &uuid)
{
    FormPage *p = new FormPage(this);
    if (!uuid.isEmpty())
        p->setUuid(uuid);
    return p;
}

void FormMain::languageChanged()
{
    qWarning() << "FormMain language changed" << uuid();
}

void FormMain::clear()
{
    foreach(FormItem *it, this->flattenFormItemChildren()) {
        if (it->itemDatas())
            it->itemDatas()->clear();
    }
}

QString FormMain::printableHtml(bool withValues) const
{
    /** \todo code here : print a form with/wo item content */
    FormMain *f = (FormMain*)this;
    return f->formWidget()->printableHtml(withValues);
}

void FormMain::createDebugPage()
{
    m_DebugPage = new FormMainDebugPage(this, this);
    ExtensionSystem::PluginManager::instance()->addObject(m_DebugPage);
}

inline static void itemToTree(FormItem *item, QTreeWidgetItem *tree)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(tree, QStringList() << item->spec()->pluginName() << item->spec()->label());
    QFont bold;
    bold.setBold(true);
    i->setFont(0,bold);
    item->valueReferences()->toTreeWidget(i);
    item->scripts()->toTreeWidget(i);
    foreach(FormItem *c, item->formItemChildren()) {
        itemToTree(c, i);
    }
}

void FormMain::toTreeWidget(QTreeWidget *tree)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(tree, QStringList() << tr("Form : ") + spec()->label());
    QFont bold;
    bold.setBold(true);
    i->setFont(0,bold);
    spec()->toTreeWidget(i);
    valueReferences()->toTreeWidget(i);
    scripts()->toTreeWidget(i);
    foreach(FormItem *c, formItemChildren()) {
        itemToTree(c, i);
    }
}

void FormMain::emitFormLoaded()
{
    Q_EMIT formLoaded();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  FormItemSpec  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpecsBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *i)
    {
        new QTreeWidgetItem(i, QStringList() << "Authors" << m_Specs.value(Form::FormItemSpec::Spec_Author).toString() );
        new QTreeWidgetItem(i, QStringList() << "License" << m_Specs.value(Form::FormItemSpec::Spec_License).toString() );
        new QTreeWidgetItem(i, QStringList() << "version" << m_Specs.value(Form::FormItemSpec::Spec_Version).toString() );
        new QTreeWidgetItem(i, QStringList() << "biblio" << m_Specs.value(Form::FormItemSpec::Spec_Bibliography).toString() );
        new QTreeWidgetItem(i, QStringList() << "Description" << m_Specs.value(Form::FormItemSpec::Spec_Description).toString() );
        new QTreeWidgetItem(i, QStringList() << "category" << m_Specs.value(Form::FormItemSpec::Spec_Category).toString() );
        new QTreeWidgetItem(i, QStringList() << "creationDate" << m_Specs.value(Form::FormItemSpec::Spec_CreationDate).toString() );
        new QTreeWidgetItem(i, QStringList() << "LastModification" << m_Specs.value(Form::FormItemSpec::Spec_LastModified).toString() );
        new QTreeWidgetItem(i, QStringList() << "Plugin Name" << m_Specs.value(Form::FormItemSpec::Spec_Plugin).toString() );
        new QTreeWidgetItem(i, QStringList() << "IconFileName" << m_Specs.value(Form::FormItemSpec::Spec_IconFileName).toString() );
    }

    QHash<int, QVariant> m_Specs;
};

namespace Form {
namespace Internal {
class FormItemSpecPrivate : public Trans::MultiLingualClass<SpecsBook>
{
public:
    FormItemSpecPrivate() {}
    ~FormItemSpecPrivate() {}

    QString categoryForTreeWiget() const {return QString("Specs");}
};

} // End Internal
} // End Form

FormItemSpec::FormItemSpec() :
        d(new Form::Internal::FormItemSpecPrivate)
{
}

FormItemSpec::~FormItemSpec()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines a value \e val for one \e type at one specified \e id in a virtual list, for one \e language.
  \e language must be a valid local language name in 2 chars encoding.
*/
void FormItemSpec::setValue(int type, const QVariant &val, const QString &language)
{
//    qWarning() << "SPEC" << type << val << language;
    QString l = language;
    if (language.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    SpecsBook *values = d->createLanguage(l);
    values->m_Specs.insert(type,val);
}

QVariant FormItemSpec::value(const int type, const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty())
        l = QLocale().name().left(2);
    SpecsBook *values = d->getLanguage(l);
    if (!values)
        return QString();
    QVariant val = values->m_Specs.value(type);
    if (val.isNull() && (l.compare(Trans::Constants::ALL_LANGUAGE)!=0)) {
        val = value(type, Trans::Constants::ALL_LANGUAGE);
    }
    return val;
}

void FormItemSpec::toTreeWidget(QTreeWidgetItem *tree) const
{
    d->toTreeWidget(tree);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FormMainDebugPage  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormMainDebugPage::FormMainDebugPage(FormMain *form, QObject *parent) :
        IDebugPage(parent), m_Form(form)
{
    setObjectName("FormMainDebugPage_" + m_Form->uuid());
    m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    tree->setColumnCount(2);
    layout->addWidget(tree);
}

FormMainDebugPage::~FormMainDebugPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString FormMainDebugPage::name() const
{ return objectName(); }

QString FormMainDebugPage::category() const
{ return tr("Forms : ") + m_Form->spec()->label(); }

void FormMainDebugPage::refreshContents()
{
    tree->clear();
    m_Form->toTreeWidget(tree);
    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

QWidget *FormMainDebugPage::createPage(QWidget *parent)
{
    return m_Widget;
}
