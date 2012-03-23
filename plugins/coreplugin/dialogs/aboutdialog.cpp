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
  \class Core::AboutDialog
  \brief This dialog shows a complete About dialog with a tabwidget containing : a short about text, a complete team list, a full license text.
   It also shows the compilation date and time and the actual version of the application. \n
   The name of the application is picked into qApp->applicationName() and the version into qApp->applicationVersion(). \n
   You can inform the team using the addTeamList().\n
   You can get the default license terms using the static getLicenseTerms() function. This function manages english and french language by looking the default QLocale()::name().\n
*/

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/iaboutpage.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include <QStackedLayout>
#include <QHeaderView>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Core::Internal::Ui::AboutDialog)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;
    m_ui->setupUi(this);
    m_ui->applicationNameLabel->setText(qApp->applicationName());

    QList<IAboutPage*> pages =
        ExtensionSystem::PluginManager::instance()->getObjects<IAboutPage>();

    m_ui->widget->setPages<IAboutPage>(pages);
    m_ui->widget->setSettingKey("Dialogs/About");
    m_ui->widget->setupUi();
    m_ui->widget->expandAllCategories();

    setWindowTitle(tkTr(Trans::Constants::ABOUT_TEXT).remove("&"));

    // resize and center window
    Utils::resizeAndCenter(this);
}

AboutDialog::~AboutDialog()
{
    m_ui->widget->saveState();
    delete m_ui;
}
