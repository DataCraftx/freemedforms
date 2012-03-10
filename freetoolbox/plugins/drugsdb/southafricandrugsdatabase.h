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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SOUTHAFRICANDRUGSDATABASE_H
#define SOUTHAFRICANDRUGSDATABASE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/ifullreleasestep.h>

#include <QWidget>
#include <QMap>
#include <QString>
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;


namespace DrugsDbCreator {

namespace Ui {
class SouthAfricanDrugsDatabase;
}


class SouthAfricanDrugsDatabasePage : public Core::IToolPage
{
public:
    SouthAfricanDrugsDatabasePage(QObject *parent) : IToolPage(parent) {}

    virtual QString id() const {return "ZADrugsDatabase";}
    virtual QString name() const {return "ZA Drugs Database Creator";}
    virtual QString category() const {return Core::Constants::CATEGORY_DRUGSDATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

};

class ZaDrugDatatabaseStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    ZaDrugDatatabaseStep(QObject *parent = 0);
    ~ZaDrugDatatabaseStep();

    QString id() const {return "ZaDrugDatatabaseStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const {return tr("South African drugs database creation");}

    bool prepareDatas();
    bool createDatabase();
    bool populateDatabase();
    bool linkMolecules();

    QStringList errors() const {return m_Errors;}

private Q_SLOTS:
    void replyFinished(QNetworkReply *reply);

private:
    QStringList m_Errors;
    QNetworkAccessManager *manager;
    QMap<QString, QString> m_Drug_Link;
    int m_nbOfDowloads;
    QProgressDialog *m_Progress;
    bool m_WithProgress;
};

class SouthAfricanDrugsDatabase : public QWidget
{
    Q_OBJECT
public:
    SouthAfricanDrugsDatabase(QWidget *parent);
    ~SouthAfricanDrugsDatabase();

protected Q_SLOTS:
    void on_startJobs_clicked();
    bool on_download_clicked();

private:
    void changeEvent(QEvent *e);

private:
    Ui::SouthAfricanDrugsDatabase *ui;
    QString m_WorkingPath;
    ZaDrugDatatabaseStep *m_Step;
};


}  //  End namespace DrugsDbCreator

#endif // SOUTHAFRICANDRUGSDATABASE_H
