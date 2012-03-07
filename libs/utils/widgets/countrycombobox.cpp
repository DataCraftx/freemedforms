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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "countrycombobox.h"
#include <utils/global.h>

#include <QLocale>
#include <QMap>
#include <QFileInfo>

#include <QDebug>

using namespace Utils;

CountryComboBox::CountryComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void CountryComboBox::setFlagPath(const QString &absPath)
{
    if (QFileInfo(absPath).exists())
        m_FlagPath = absPath;
    else
        m_FlagPath.clear();
}

void CountryComboBox::initialize()
{
    // populate with available countries
    int i=1;
    QMap<QString, int> countries;
    while (i < 246) {
        const QString &c = QLocale::countryToString(QLocale::Country(i));
        if (c.isEmpty())
            return;
        countries.insert(c, i);
        ++i;
    }
    QMapIterator<QString, int> it(countries);
    while (it.hasNext()) {
        it.next();
        QString flag = Utils::countryToIso(QLocale::Country(it.value()));
        addItem(QIcon(QString("%1/%2.png").arg(m_FlagPath).arg(flag)), it.key(), it.value());
    }
    setCurrentIndex(QLocale().system().country()-1);
}

QLocale::Country CountryComboBox::currentCountry() const
{
    return QLocale::Country(currentIndex()-1);
}

QString CountryComboBox::currentIsoCountry() const
{
    int country = itemData(currentIndex()).toInt();
    return Utils::countryToIso(QLocale::Country(country)).toUpper();
}

void CountryComboBox::setCurrentIsoCountry(const QString &isoCode)
{
    for(int i=0; i < count(); ++i) {
        int country = itemData(i).toInt();
        const QString &iso = Utils::countryToIso(QLocale::Country(country)).toUpper();
        if (iso == isoCode.toUpper()) {
            setCurrentIndex(i);
            break;
        }
    }
}

void CountryComboBox::setCurrentCountry(QLocale::Country country)
{
    setCurrentIndex(country+1);
}
