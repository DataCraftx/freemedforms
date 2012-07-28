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
/**
  \namespace Views
  \brief Contains some specific views and models to use with the views.
*/

#include "listviewplugin.h"
#include "viewmanager.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Views;

ListViewPlugin::ListViewPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating ListViewPlugin";
}

ListViewPlugin::~ListViewPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool ListViewPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ListViewPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    Internal::ViewManager::instance(this);
    return true;
}

void ListViewPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ListViewPlugin::extensionsInitialized";
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}


Q_EXPORT_PLUGIN(ListViewPlugin)
