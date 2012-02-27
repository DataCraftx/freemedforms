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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
 * \class DataPack::ServerModel
 * Represents a model which contains the ServerManager instance's servers.
 * This model is a read only model. If you need to add/remove servers you have to work with
 * the DataPack::IServerManager instance. \sa DataPack::DataPackCore::serverManager()
 */

#include "servermodel.h"
#include "datapackcore.h"
#include "iservermanager.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <QIcon>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {
const char * const ICON_SERVER_CONNECTED = "connect_established.png";
const char * const ICON_SERVER_NOT_CONNECTED = "connect_no.png";
const char * const ICON_SERVER_ASKING_CONNECTION = "connect_creating.png";
const char * const ICON_SERVER_LOCAL = "server-local.png";
}

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IServerManager *serverManager() { return core().serverManager(); }
static inline QIcon icon(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return QIcon(DataPackCore::instance().icon(name, path)); }

ServerModel::ServerModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(serverManager(), SIGNAL(serverAdded(int)), SLOT(serverAdded(int)));
    connect(serverManager(), SIGNAL(serverRemoved(int)), SLOT(serverRemoved(int)));
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), SLOT(allServerDescriptionAvailable()));
}

int ServerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return DataPackCore::instance().serverManager()->serverCount();
}

QVariant ServerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Server &s = serverManager()->getServerAt(index.row());
    if (s.isNull())
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        {
            switch (index.column()) {
            case PlainTextLabel: return s.label();
            case HtmlLabel:
            {
                QString label = s.label();
                if (s.label().isEmpty())
                    label = tkTr(Trans::Constants::UNKNOWN_SERVER);
                label = QString("<span style=\"color:black;font-weight:bold;\">%1</span>")
                        .arg(label);

                if (s.isConnected()) {
                    label += QString("<br /><span style=\"color:gray; font-size:small;\">%2 (%3: %4)</span>")
                            .arg(tkTr(Trans::Constants::CONNECTED))
                            .arg(tkTr(Trans::Constants::LAST_CHECK))
                            .arg(s.lastChecked().toString(QLocale().dateFormat(QLocale::LongFormat)));
                } else {
                    label += QString("<br /><small>%2</small>")
                            .arg(tkTr(Trans::Constants::NOT_CONNECTED));
                }

                label += QString("<br /><span style=\"color:gray; font-size:small;\">%1 %2</span>")
                        .arg(serverManager()->getPackForServer(s).count())
                        .arg(tkTr(Trans::Constants::PACKAGES));
                return label;
            }
            case Uuid : return s.uuid();
            case Authors: return s.description().data(ServerDescription::Author);
            case Version: return s.version();
            case NativeUrl: return s.nativeUrl();
            case CreationDate: return s.description().data(ServerDescription::CreationDate).toDate().toString(QLocale().dateFormat());
            case Vendor:
                if (s.description().data(ServerDescription::Vendor).toString().isEmpty())
                    return tkTr(Trans::Constants::THE_FREEMEDFORMS_COMMUNITY);
                else
                    return s.description().data(ServerDescription::Vendor);
            case RecommendedUpdateFrequencyIndex: return s.recommendedUpdateFrequency();
            case HtmlDescription: return s.description().data(ServerDescription::HtmlDescription);
            case LastUpdateDate: return s.description().data(ServerDescription::LastModificationDate).toDate().toString(QLocale().dateFormat());
            }// End switch
        }
    case Qt::ToolTipRole:
        {
            QString toolTip = QString("<b>%1</b>:&nbsp;%2<br/><b>%3</b>:&nbsp;%4<br/><b>%5</b>:&nbsp;%6")
                .arg(tr("Native Url").replace(" ", "&nbsp;"))
                .arg(s.nativeUrl())
                .arg(tr("Recommended update checking"))
                .arg(Trans::ConstantTranslations::checkUpdateLabel(s.recommendedUpdateFrequency()))
                .arg(tr("Url Style"))
                .arg(s.urlStyleName());
            return toolTip.replace(" ", "&nbsp;");
        }
    case Qt::DecorationRole:
        if (s.isLocalServer())
            return icon(::ICON_SERVER_LOCAL);
        if (s.isConnected())
            return icon(::ICON_SERVER_CONNECTED);
        return icon(::ICON_SERVER_NOT_CONNECTED);
    default: return QVariant();
    }

    return QVariant();
}

void ServerModel::serverAdded(int row)
{
    beginInsertRows(QModelIndex(), row, row);
    endInsertRows();
}

void ServerModel::serverRemoved(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    endInsertRows();
}

void ServerModel::allServerDescriptionAvailable()
{
    // Just emit data changed of all rows
    Q_EMIT dataChanged(index(0,0), index(rowCount(),0));
}
