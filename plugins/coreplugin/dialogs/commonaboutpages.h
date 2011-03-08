/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef COMMONABOUTPAGES_H
#define COMMONABOUTPAGES_H

#include <coreplugin/iaboutpage.h>
class QTreeWidgetItem;

namespace Core {
namespace Internal {

class BuildAboutPage : public IAboutPage
{
    Q_OBJECT
public:
    BuildAboutPage(QObject *parent);
    ~BuildAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("4. Build informations"); }
    QString category() const { return tr("Application"); }

    QWidget *widget();
};

class LicenseAboutPage : public IAboutPage
{
    Q_OBJECT
public:
    LicenseAboutPage(QObject *parent);
    ~LicenseAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("3. License"); }
    QString category() const { return tr("Application"); }

    QWidget *widget();
};

class TeamAboutPage : public IAboutPage
{
    Q_OBJECT
public:
    TeamAboutPage(QObject *parent);
    ~TeamAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("2. Team"); }
    QString category() const { return tr("Application"); }

    QWidget *widget();

private Q_SLOTS:
    void mailTo(QTreeWidgetItem *item);
};



} // End Internal
} // End Core

#endif // COMMONABOUTPAGES_H
