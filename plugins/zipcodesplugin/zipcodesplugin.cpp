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
#include "zipcodesplugin.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace ZipCodes;

ZipCodesPlugin::ZipCodesPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating ZipCodesPlugin";

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
}

ZipCodesPlugin::~ZipCodesPlugin()
{
}

bool ZipCodesPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ZipCodesPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // No user connected here

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("zipcodesplugin");

    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool

    return true;
}

void ZipCodesPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ZipCodesPlugin::extensionsInitialized";

    // At this point, user is connected

    // All preferences pages must be created in this part (after user connection)

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}



Q_EXPORT_PLUGIN(ZipCodesPlugin)
