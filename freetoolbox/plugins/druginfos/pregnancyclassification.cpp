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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pregnancyclassification.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/httpdownloader.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <QScriptEngine>
#include <QDir>
#include <QUrl>

#include "ui_pregnancyclassification.h"

using namespace DrugInfos;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/TgaPregDb/") + QDir::separator();}
static inline QString databaseAbsPath()  {return Core::Tools::drugsDatabaseAbsFileName();}

static inline QString databaseDescriptionFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/tga_preg/description.xml");}

QString PregnancyClassificationPage::id() const {return "PregnancyClassificationPage";}
QString PregnancyClassificationPage::name() const {return "Pregnancy & drugs";}
QIcon PregnancyClassificationPage::icon() const {return QIcon();}
QString PregnancyClassificationPage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGINFOSDATABASE;
}

namespace {

const char* const  PREGNANCY_TGA_URL            = "http://www.tga.gov.au/webfiles/medicinesInPregnancyData.js";
const char* const  TGA_PREGNANCY_DATABASE_NAME  = "TGA_PREG";

enum FieldType {
    Field_Name,
    Field_Category,
    Field_Safety,
    Field_Class1,
    Field_Class2,
    Field_Class3
};

typedef QHash<FieldType, QString> PregnancyRecord;

/**
 * Load a pregnancy db file.
 * \param {QString} fileName The .js pregnancy filename
 * \param {QList<PregnancyRecord>} records a reference to a list of records in which all read records will be loaded into
 * \param {QString} [errorMsg] An optional QString used to get the error message if loading failed
 * \return true if success, false if failed
 */
bool load(const QString &fileName, QList<PregnancyRecord> &records, QString *errorMsg = 0)
{
    QFile scriptFile(fileName);
    if (!scriptFile.open(QIODevice::ReadOnly)) {
        if (errorMsg)
            *errorMsg = QString("Error while opening %1").arg(fileName);
        return false;
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    QScriptEngine myEngine;
    QScriptValue fun = myEngine.evaluate("(" + contents + ")", fileName);

    if (!fun.isValid() || !fun.isFunction()) {
        if (errorMsg)
            *errorMsg = QString("Error while evaluating the function inside %1").arg(fileName);
        return false;
    }

    QScriptValue obj = fun.call();

    if (!obj.isValid() || !obj.isObject()) {
        if (errorMsg)
            *errorMsg = QString("Error: the function does not return a valid object");
        return false;
    }

    records.clear();
    QVariantMap drugsMap = obj.toVariant().toMap();

    foreach (const QString &key, drugsMap.keys()) {
        QVariant val = drugsMap[key];
        if (val.toList().empty())
            continue;
        QVariantMap rMap = val.toList()[0].toMap();
        PregnancyRecord rec;
        rec.insert(Field_Name, key);
        rec.insert(Field_Category, rMap["Category"].toString());
        rec.insert(Field_Safety, rMap["Safety"].toString());
        rec.insert(Field_Class1, rMap["Class1"].toString());
        rec.insert(Field_Class2, rMap["Class2"].toString());
        rec.insert(Field_Class3, rMap["Class3"].toString());
        records << rec;
    }

    return true;
}

} // End namespace Anonymous

// widget will be deleted after the show
QWidget *PregnancyClassificationPage::createPage(QWidget *parent)
{
    return new PregnancyClassificationWidget(parent);
}

PregnancyDatatabaseStep::PregnancyDatatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false)
{
    setObjectName("PregnancyDatatabaseStep");
}

PregnancyDatatabaseStep::~PregnancyDatatabaseStep()
{
}

bool PregnancyDatatabaseStep::createDir()
{
    Utils::checkDir(workingPath(), true, "PregnancyDatatabaseStep::createDir");
    Utils::checkDir(QFileInfo(databaseAbsPath()).absolutePath(), true, "PregnancyDatatabaseStep::createDir");
    return true;
}

bool PregnancyDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool PregnancyDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
//    dld->setMainWindow(mainwindow());
    dld->setProgressBar(bar);
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(PREGNANCY_TGA_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    connect(dld, SIGNAL(downloadProgressRange(qint64,qint64)), this, SIGNAL(progressRangeChanged(qint64,qint64)));
//    connect(dld, SIGNAL(downloadProgressRead(qint64)), this, SIGNAL(progress(int)));
    return true;
}

bool PregnancyDatatabaseStep::process()
{
    unzipFiles();
    prepareDatas();
    createDatabase();
    populateDatabase();
//    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool PregnancyDatatabaseStep::unzipFiles()
{
    Q_EMIT progressLabelChanged(tr("Unzipping downloaded files"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);
    return true;
}

bool PregnancyDatatabaseStep::prepareDatas()
{
    return true;
}

bool PregnancyDatatabaseStep::createDatabase()
{
    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;
    return true;
}

bool PregnancyDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    Q_EMIT progressLabelChanged(tr("Reading downloaded files"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    QList<QHash<FieldType, QString> > pregnancyList;
    QString errorMsg;
    // NOTE POUR ERIC: change le chemin du fichier par le tien
    QString jsFile = workingPath() + QFileInfo(::PREGNANCY_TGA_URL).fileName();
    if (load(jsFile, pregnancyList, &errorMsg)) {
        qDebug("SUCCESS");
        foreach (const PregnancyRecord &rec, pregnancyList) {
            qDebug("%s, %s, %s, %s, %s, %s", qPrintable(rec[Field_Name]),
                   qPrintable(rec[Field_Category]),
                   qPrintable(rec[Field_Safety]),
                   qPrintable(rec[Field_Class1]),
                   qPrintable(rec[Field_Class2]),
                   qPrintable(rec[Field_Class3]));
        }
    } else {
        qDebug("FAILURE: %s", qPrintable(errorMsg));
        return false;
    }
    return true;
}




PregnancyClassificationWidget::PregnancyClassificationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PregnancyClassificationWidget)
{
    ui->setupUi(this);
    ui->progressBar->setRange(0,1);
    ui->progressBar->setValue(0);
    m_Step = new PregnancyDatatabaseStep(this);
    m_Step->createDir();
//    computeJavascriptFile();
}

PregnancyClassificationWidget::~PregnancyClassificationWidget()
{
}

void PregnancyClassificationWidget::computeJavascriptFile()
{
    m_Step->populateDatabase();
}

void PregnancyClassificationWidget::on_download_clicked()
{
    ui->progressBar->show();
    m_Step->downloadFiles(ui->progressBar);
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
}

void PregnancyClassificationWidget::downloadFinished()
{
    ui->progressBar->hide();
    ui->download->setEnabled(true);
}

void PregnancyClassificationWidget::on_editClassification_clicked()
{
}

void PregnancyClassificationWidget::on_startJobs_clicked()
{
    m_Step->populateDatabase();
}
