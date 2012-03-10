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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGBASECORE_H
#define DRUGBASECORE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

/**
 * \file drugsbasecore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 13 Feb 2012
*/

namespace DataPack {
class Pack;
}

namespace DrugsDB {
class DrugsBase;
class ProtocolsBase;
class InteractionManager;
class VersionUpdater;

namespace Internal {
class DrugBaseCorePrivate;
}

class DRUGSBASE_EXPORT DrugBaseCore : public QObject
{
    Q_OBJECT
    DrugBaseCore(QObject *parent = 0);
    bool init();

public:
    static DrugBaseCore &instance(QObject *parent = 0);
    virtual ~DrugBaseCore();

    DrugsBase &drugsBase() const;
    ProtocolsBase &protocolsBase() const;
    InteractionManager &interactionManager() const;
    VersionUpdater &versionUpdater() const;

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void packChanged(const DataPack::Pack &pack);

private:
    static DrugBaseCore *m_Instance;
    Internal::DrugBaseCorePrivate *d;
};

}  // End namespace DrugsDB

#endif // DRUGBASECORE_H
