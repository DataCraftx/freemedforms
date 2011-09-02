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
 ***************************************************************************/
#include "zipcodesplugin.h"
#include "frenchzipcodespage.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

/** \todo Ca zipcodes: http://www.populardata.com/postalcode_database.html */
/** \todo USA zipcodes: http://www.populardata.com/zipcode_database.html // http://federalgovernmentzipcodes.us/ //  http://www.free-zipcodes.com/download-zip-code-database.php */
// UK: http://www.freemaptools.com/download-uk-postcode-lat-lng.htm
// Australia http://auspost.com.au/apps/postcode.html
// ALL !!!! : http://download.geonames.org/export/dump/ (no zip codes)

using namespace ZipCodes;
using namespace Internal;

ZipCodesPlugin::ZipCodesPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Creating ZipCodesPlugin";
}

ZipCodesPlugin::~ZipCodesPlugin()
{
    qWarning() << "ZipCodesPlugin::~ZipCodesPlugin()";
}

bool ZipCodesPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ZipCodesPlugin::initialize";

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addAutoReleasedObject(new FrenchZipCodesPage(this));

    return true;
}

void ZipCodesPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "ZipCodesPlugin::extensionsInitialized";
}


Q_EXPORT_PLUGIN(ZipCodesPlugin)
