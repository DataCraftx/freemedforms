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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FDADRUGSDATABASECREATOR_H
#define FDADRUGSDATABASECREATOR_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/ifullreleasestep.h>

#include <QWidget>


namespace DrugsDbCreator {

class FdaDrugsDatabasePage : public Core::IToolPage
{
public:
    FdaDrugsDatabasePage(QObject *parent = 0);

    virtual QString id() const {return "FdaDrugsDatabase";}
    virtual QString name() const {return "FDA Drugs Database Creator";}
    virtual QString category() const {return Core::Constants::CATEGORY_DRUGSDATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

class FdaDrugDatatabaseStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    FdaDrugDatatabaseStep(QObject *parent = 0);
    ~FdaDrugDatatabaseStep();

    QString id() const {return "FdaDrugDatatabaseStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles();
    bool process();
    QString processMessage() const {return tr("USA drugs database creation");}

    bool unzipFiles();
    bool prepareDatas();
    bool createDatabase();
    bool populateDatabase();
    bool linkDrugsRoutes();
    bool linkMolecules();

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
    bool m_WithProgress;

};


namespace Ui {
    class FdaDrugsDatabaseWidget;
}

class FdaDrugsDatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FdaDrugsDatabaseWidget(QWidget *parent = 0);
    ~FdaDrugsDatabaseWidget();

protected Q_SLOTS:
    void on_startJobs_clicked();
    bool on_download_clicked();
    void downloadFinished();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FdaDrugsDatabaseWidget *ui;
    FdaDrugDatatabaseStep *m_Step;
};

}  //  End namespace DrugsDbCreator


#endif // FDADRUGSDATABASECREATOR_H
