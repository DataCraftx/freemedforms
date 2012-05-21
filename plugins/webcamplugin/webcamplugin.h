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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef WEBCAM_H
#define WEBCAM_H

#include "webcam_exporter.h"
#include <webcamphotoprovider.h>
#include <extensionsystem/iplugin.h>

namespace Webcam {
namespace Internal {

class WebcamPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    WebcamPlugin();
    ~WebcamPlugin();
    
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    //    ShutdownFlag aboutToShutdown();
    
private:
    WebcamPhotoProvider *m_webcamProvider;

private Q_SLOTS:
    void postCoreInitialization();
    void coreAboutToClose();
    //    void triggerAction();
};

} // namespace Internal
} // namespace Webcam

#endif // WEBCAM_H

