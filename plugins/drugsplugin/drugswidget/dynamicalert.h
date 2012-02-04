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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DYNAMICALERT_H
#define DYNAMICALERT_H

#include <drugsplugin/drugs_exporter.h>

#include <QDialog>

/**
 * \file dynamicalert.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 14 Mar 2011
 */

namespace DrugsDB {
class DrugInteractionInformationQuery;
}

namespace DrugsWidget {

namespace Ui {
    class DynamicAlert;
}

class DRUGS_EXPORT DynamicAlert : public QDialog
{
    Q_OBJECT

    explicit DynamicAlert(const DrugsDB::DrugInteractionInformationQuery &query, QWidget *parent = 0);
public:
    enum DialogResult {
        NoDynamicAlert = 0,
        DynamicAlertOverridden,
        DynamicAlertAccepted
    };
    ~DynamicAlert();

    static DialogResult executeDynamicAlert(const DrugsDB::DrugInteractionInformationQuery &query, QWidget *parent = 0);

protected Q_SLOTS:
    void showInteractionSynthesisDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DynamicAlert *ui;
};

}  // End namespace DrugsWidget
#endif // DYNAMICALERT_H
