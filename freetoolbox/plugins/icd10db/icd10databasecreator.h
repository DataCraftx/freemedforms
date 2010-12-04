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
#ifndef ICD10DATABASECREATOR_H
#define ICD10DATABASECREATOR_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
#include <QProgressDialog>

namespace Utils{
class HttpDownloader;
}

namespace Icd10 {

class Icd10DatabasePage : public Core::IToolPage
{
public:
    Icd10DatabasePage(QObject *parent = 0);

    virtual QString id() const {return "Icd10DatabasePage";}
    virtual QString name() const {return "ICD10 Database Creator";}
    virtual QString category() const {return Core::Constants::CATEGORY_ICD10DATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


namespace Ui {
    class Icd10DatabaseWidget;
}

class Icd10DatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Icd10DatabaseWidget(QWidget *parent = 0);
    ~Icd10DatabaseWidget();

protected Q_SLOTS:
    bool on_startCreation_clicked();
    bool downloadRawSources();
    bool downloadFinished();
    bool populateDatabaseWithRawSources();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Icd10DatabaseWidget *ui;
    QString m_WorkingPath;
    Utils::HttpDownloader *m_Downloader;
    QProgressDialog *m_Progress;
};

}  //  End namespace Icd10


#endif // ICD10DATABASECREATOR_H
