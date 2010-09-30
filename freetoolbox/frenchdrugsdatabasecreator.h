/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FRENCHDRUGSDATABASECREATOR_H
#define FRENCHDRUGSDATABASECREATOR_H

#include "itoolpage.h"
#include "constants.h"

#include <QWidget>
#include <QMultiHash>

class FrenchDrugsDatabasePage : public IToolPage
{
    Q_OBJECT
public:
    explicit FrenchDrugsDatabasePage(QObject *parent = 0);

    virtual QString id() const {return "FrenchDrugsDatabase";}
    virtual QString name() const {return "French Drugs Database Creator";}
    virtual QString category() const {return Constants::CATEGORY_DRUGSDATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


namespace Ui {
    class FrenchDrugsDatabaseWidget;
}

class FrenchDrugsDatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FrenchDrugsDatabaseWidget(QWidget *parent = 0);
    ~FrenchDrugsDatabaseWidget();

protected Q_SLOTS:
    void on_startJobs_clicked();

private:
    bool downloadFiles();
    bool unzipFiles();
    bool prepareDatas();
    bool createDatabase();
    bool populateDatabase();
    bool linkMolecules();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FrenchDrugsDatabaseWidget *ui;
    QString m_WorkingPath;
};



#endif // FRENCHDRUGSDATABASECREATOR_H
