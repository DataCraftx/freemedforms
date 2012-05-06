/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Patients::PatientSearchMode
  \brief Core::IMode for the Patient selector.
  \sa Core::IMode
*/

#include "patientsearchmode.h"
#include "patientselector.h"
#include "patientwidgetmanager.h"
#include "constants_menus.h"

#include <coreplugin/icore.h>
#include <coreplugin/theme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constanttranslations.h>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

PatientSearchMode::PatientSearchMode(QObject *parent) :
        Core::IMode(parent), m_Selector(0)
{
    m_Selector = new PatientSelector;
    m_Selector->setFieldsToShow(PatientSelector::Gender | PatientSelector::BirthName | PatientSelector::SecondName | PatientSelector::FirstName | PatientSelector::DateOfBirth | PatientSelector::FullAdress);
    PatientWidgetManager::instance()->setCurrentView(m_Selector);
    Core::Command *cmd = actionManager()->command(Core::Constants::A_PATIENT_NEW);
    modeManager()->addAction(cmd, Core::Constants::P_MODE_PATIENT_SEARCH);
}

PatientSearchMode::~PatientSearchMode()
{
    if (m_Selector) {
        delete m_Selector;
        m_Selector = 0;
    }
}

QString PatientSearchMode::name() const
{
    return tkTr(Trans::Constants::PATIENTS);
}

QIcon PatientSearchMode::icon() const
{
    return theme()->icon(Core::Constants::ICONPATIENTS, Core::ITheme::BigIcon);
}

int PatientSearchMode::priority() const
{
    return Core::Constants::P_MODE_PATIENT_SEARCH;
}

QWidget* PatientSearchMode::widget()
{
    return m_Selector;
}

const char* PatientSearchMode::uniqueModeName() const
{
    return Core::Constants::MODE_PATIENT_SEARCH;
}

QList<int> PatientSearchMode::context() const
{
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    static QList<int> contexts = QList<int>() <<
                                 uid->uniqueIdentifier(Constants::C_PATIENTS) <<
                                 uid->uniqueIdentifier(Constants::C_PATIENTS_SEARCH);
    return contexts;
}

