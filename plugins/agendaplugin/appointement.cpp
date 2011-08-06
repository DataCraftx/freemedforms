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
#include "appointement.h"
#include "calendaritemmodel.h"

#include <calendar/common.h>

using namespace Agenda;
using namespace Internal;

/** Constructs an invalid item */
Appointement::Appointement() :
    m_Modified(false),
    m_uid(-1)
{
    m_Datas.insert(Constants::Db_CalId, -1);
    m_Datas.insert(Constants::Db_ComId, -1);
    m_Datas.insert(Constants::Db_EvId, -1);
    m_Datas.insert(Constants::Db_CyclingEvId, -1);
    m_Datas.insert(Constants::Db_UserCalId, -1);
    m_Datas.insert(Constants::Db_IsValid, false);
}

bool Appointement::isNull() const
{
    if (m_Modified)
        return false;
    if ((m_Datas.value(Constants::Db_CalId).toInt() == -1) &&
            (m_Datas.value(Constants::Db_ComId).toInt() == -1) &&
            (m_Datas.value(Constants::Db_EvId).toInt() == -1) &&
            (m_Datas.value(Constants::Db_CyclingEvId).toInt() == -1) &&
            (m_Datas.value(Constants::Db_UserCalId).toInt() == -1) &&
            (m_Datas.value(Constants::Db_IsValid).toBool() == false)) {
        return true;
    }
    return false;
}

bool Appointement::isValid() const
{
    /** \todo code here */
    return true;
}

QVariant Appointement::data(const int ref) const
{
    /** \todo retrieve datas from the model */
    return m_Datas.value(ref, QVariant());
}

bool Appointement::setData(const int ref, const QVariant &value)
{
    /** \todo set datas to the model */
    m_Modified = true;
    m_Datas.insert(ref, value);
    return true;
}

QDateTime Appointement::beginning() const
{
    return data(CalendarItemModel::DateStart).toDateTime();
}

QDateTime Appointement::ending() const
{
    return data(CalendarItemModel::DateEnd).toDateTime();
}

/** compute an intersection value with a day range
             * returns:
             * -1 if item is entirely before first day
             * 0 if item intersects [firstDay, lastDay]
             * 1 if item is entirely after lastDay
*/
int Appointement::intersects(const QDate &firstDay, const QDate &lastDay) const
{
    return Calendar::intersectsDays(beginning(), ending(), firstDay, lastDay);
}


bool Appointement::dateLessThan(const Appointement *item1, const Appointement *item2)
{
    // at first compare with begin dates. If they're equals, compare by end dates.
    // at first, compare the beginnings
    if (item1->beginning() < item2->beginning())
        return true;
    else if (item1->beginning() > item2->beginning())
        return false;
    // beginnings are the same => compare the endings
    else if (item1->ending() > item2->ending())
        return true;
    else if (item1->ending() < item2->ending())
        return false;
//    // beginnings and endings are the same, compare the creation date time
//    else if (item1.created() < item2.created())
//        return true;
//    else if (item1.created() > item2.created())
//        return false;
//    // create date time are the same => finally compare uid
//    else
//        return item1.uid() > item2.uid();
    return false;
}


