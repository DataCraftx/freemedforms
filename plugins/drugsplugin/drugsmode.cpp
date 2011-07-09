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
#include "drugsmode.h"

#include <formmanagerplugin/formplaceholder.h>
#include <formmanagerplugin/formmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <extensionsystem/pluginmanager.h>

#include <QDebug>

using namespace DrugsWidget;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager *formManager() {return Form::FormManager::instance();}

DrugsMode::DrugsMode(QObject *parent) :
        Core::BaseMode(parent),
        inPool(false),
        m_Holder(0)
{
    m_Holder = new Form::FormPlaceHolder;
    m_Holder->setObjectName("DrugsFormPlaceHolder");
    setName(tr("Drugs"));
    setIcon(theme()->icon(Core::Constants::ICONDRUGMODE, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_DRUGS);
    setUniqueModeName(Core::Constants::MODE_PATIENT_DRUGS);
//    const QList<int> &context;
//    setContext();
    setWidget(m_Holder);

    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(getPatientForm()));
}

DrugsMode::~DrugsMode()
{
    if (inPool)
        pluginManager()->removeObject(this);
}

void DrugsMode::getPatientForm()
{
//    qWarning() << Q_FUNC_INFO;
    Form::FormMain *root = formManager()->rootForm(Core::Constants::MODE_PATIENT_DRUGS);
    if (!root) {
        if (inPool)
            pluginManager()->removeObject(this);
        inPool = false;
    } else {
        if (!inPool)
            pluginManager()->addObject(this);
        inPool = true;
    }
    m_Holder->setRootForm(root);
}
