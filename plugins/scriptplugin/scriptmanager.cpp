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
 ***************************************************************************/
#include "scriptmanager.h"
#include "scriptwrappers.h"
#include "scriptpatientwrapper.h"
#include "uitools.h"

#include <coreplugin/icore.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>

#include <utils/log.h>

#include <QDebug>

using namespace Script;

static inline Form::FormManager *formManager() {return Form::FormManager::instance();}

namespace {
const char * const SCRIPT_NAMESPACE =
        "/* Namespace.js - modular namespaces in JavaScript"
        "   by Mike Koss - placed in the public domain"
        "   https://github.com/mckoss/namespace"
        "*/"
        "(function(global) {"
        "    var globalNamespace = global['namespace'];"
        "    var VERSION = '3.0.1';"
        "    function Module() {}"
        "    function numeric(s) {"
        "        if (!s) {"
        "            return 0;"
        "        }"
        "        var a = s.split('.');"
        "        return 10000 * parseInt(a[0]) + 100 * parseInt(a[1]) + parseInt(a[2]);"
        "    }"
        "    if (globalNamespace) {"
        "        if (numeric(VERSION) <= numeric(globalNamespace['VERSION'])) {"
        "            return;"
        "        }"
        "        Module = globalNamespace.constructor;"
        "    } else {"
        "        global['namespace'] = globalNamespace = new Module();"
        "    }"
        "    globalNamespace['VERSION'] = VERSION;"
        "    function require(path) {"
        "        path = path.replace(/-/g, '_');"
        "        var parts = path.split('.');"
        "        var ns = globalNamespace;"
        "        for (var i = 0; i < parts.length; i++) {"
        "            if (ns[parts[i]] === undefined) {"
        "                ns[parts[i]] = new Module();"
        "            }"
        "            ns = ns[parts[i]];"
        "        }"
        "        return ns;"
        "    }"
        "    var proto = Module.prototype;"
        "    proto['module'] = function(path, closure) {"
        "        var exports = require(path);"
        "        if (closure) {"
        "            closure(exports, require);"
        "        }"
        "        return exports;"
        "    };"
        "    proto['extend'] = function(exports) {"
        "        for (var sym in exports) {"
        "            if (exports.hasOwnProperty(sym)) {"
        "                this[sym] = exports[sym];"
        "            }"
        "        }"
        "    };"
        "}(this));";

const char * const SCRIPT_FREEMEDFORMS_NAMESPACE_CREATION =
        "namespace.module('com.freemedforms', function (exports, require) {"
        "var forms;"
        "var patient;"
        "var uiTools;"
        "  exports.extend({"
        "    'forms': forms,"
        "    'patient': patient,"
        "    'uiTools': uiTools"
        "  });"
        "});"
        "var freemedforms = namespace.com.freemedforms;";

}

QScriptValue FormItemScriptWrapperToScriptValue(QScriptEngine *engine, FormItemScriptWrapper* const &in)
{ return engine->newQObject(in); }

void FormItemScriptWrapperFromScriptValue(const QScriptValue &object, FormItemScriptWrapper* &out)
{ out = qobject_cast<FormItemScriptWrapper*>(object.toQObject()); }

ScriptManager::ScriptManager(QObject *parent) :
    Core::IScriptManager(parent),
    m_Engine(new QScriptEngine(this))
{
    // Inject default scripts
    evaluate(SCRIPT_NAMESPACE);
    evaluate(SCRIPT_FREEMEDFORMS_NAMESPACE_CREATION);

    // Add the patient
    patient = new ScriptPatientWrapper(this);
    QScriptValue patientValue = m_Engine->newQObject(patient, QScriptEngine::QtOwnership);
    m_Engine->evaluate("namespace.com.freemedforms").setProperty("patient", patientValue);

    // Add the form manager
    forms = new FormManagerScriptWrapper(this);
    QScriptValue formsValue = m_Engine->newQObject(forms, QScriptEngine::QtOwnership);
    m_Engine->evaluate("namespace.com.freemedforms").setProperty("forms", formsValue);

    // Add meta types
    qScriptRegisterMetaType<Script::FormItemScriptWrapper*>(m_Engine, ::FormItemScriptWrapperToScriptValue, ::FormItemScriptWrapperFromScriptValue);

    // Add UiTools
    tools = new UiTools(this);
    QScriptValue toolsValue = m_Engine->newQObject(tools, QScriptEngine::QtOwnership);
    m_Engine->evaluate("namespace.com.freemedforms").setProperty("uiTools", toolsValue);

    // Register to Core::ICore
    Core::ICore::instance()->setScriptManager(this);

    // Connect to formmanager
    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onAllFormsLoaded()));
    connect(formManager(), SIGNAL(subFormLoaded(QString)), this, SLOT(onSubFormLoaded(QString)));
}

QScriptValue ScriptManager::evaluate(const QString &script)
{
    if (script.isEmpty())
        return QScriptValue();
//    qWarning() << "xxxxxxxxxxxxxxxxxxxx \n\n" << script << "\n\n";
//    qWarning() << "xxxxxxxxxxxxxxxxxxxx \n\n" << script << m_Engine->evaluate(script).toVariant() << "\n\n";
    // TODO: improvement script debugging
    QScriptSyntaxCheckResult check = m_Engine->checkSyntax(script);
    if (check.state()!=QScriptSyntaxCheckResult::Valid) {
        LOG_ERROR(QString("Script error (%1;%2): ").arg(check.errorLineNumber()).arg(check.errorColumnNumber()) + check.errorMessage());
        return false;
    }
    QScriptValue val = m_Engine->evaluate(script);
    if (m_Engine->hasUncaughtException()) {
         int line = m_Engine->uncaughtExceptionLineNumber();
         LOG_ERROR("uncaught exception at line" + QString::number(line) + ":" + val.toString());
     }
    return val;
}

QScriptValue ScriptManager::addScriptObject(QObject *object)
{
    return m_Engine->newQObject(object);
}

void ScriptManager::onAllFormsLoaded()
{
    // Is equivalent to onPatientChanged (because all forms are reloaded when patient changed)
    forms->recreateItemWrappers();

    // Execute RootForm all OnLoad scripts
    foreach(Form::FormMain *main, formManager()->forms()) {
        evaluate(main->scripts()->onLoadScript());
        QList<Form::FormMain *> children = main->flattenFormMainChildren();
        foreach(Form::FormMain *mainChild, children) {
            evaluate(mainChild->scripts()->onLoadScript());
            foreach(Form::FormItem *item, mainChild->flattenFormItemChildren()) {
                evaluate(item->scripts()->onLoadScript());
            }
        }
    }
    // Execute empty root SubForms OnLoad scripts
    foreach(Form::FormMain *main, formManager()->subFormsEmptyRoot()) {
        evaluate(main->scripts()->onLoadScript());
    }
}

void ScriptManager::onSubFormLoaded(const QString &subFormUuid)
{
    // Update wrapper items
    forms->updateSubFormItemWrappers(subFormUuid);

    // Execute onload scripts of subform items only
    foreach(Form::FormMain *main, formManager()->subFormsEmptyRoot()) {
        if (main->uuid()!=subFormUuid)
            continue;

        evaluate(main->scripts()->onLoadScript());
        QList<Form::FormMain *> children = main->flattenFormMainChildren();
        foreach(Form::FormMain *mainChild, children) {
            evaluate(mainChild->scripts()->onLoadScript());
            foreach(Form::FormItem *item, mainChild->flattenFormItemChildren()) {
                evaluate(item->scripts()->onLoadScript());
            }
        }
    }
}
