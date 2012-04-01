/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "assetsViewer.h"
#include "assetsmanager.h"
#include "assetsIO.h"
#include "ui_assetsviewer.h"

#include <accountbaseplugin/assetmodel.h>
#include <accountbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QDebug>

enum { WarnDebugMessage = true };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }

AssetsViewer::AssetsViewer(QWidget * parent):QWidget(parent),ui(new Ui::AssetsViewerWidget){
    ui->setupUi(this);
    ui->dateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));

    //instanciate
    ui->valueDoubleSpinBox->setRange(0.00,999999999999999.00);
    ui->recordButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->deleteButton->setShortcut(QKeySequence::Delete);
    ui->quitButton->setShortcut(QKeySequence::Quit);
    fillModeComboBox();
    fillBankComboBox();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->yearEdit->setDate(QDate::currentDate());
    ui->yearEdit->setDisplayFormat("yyyy");
    QDate thisYear = ui->yearEdit->date();
    refresh();
    showAssets();
    yearDateChanged(thisYear);
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordAsset()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteAsset()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->yearEdit,SIGNAL(dateChanged(const QDate&)),this,SLOT(yearDateChanged(const QDate&)));
    connect(ui->tableView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(writeLabelByRow(const QModelIndex&)));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userIsChanged()));
}

AssetsViewer::~AssetsViewer(){}

void AssetsViewer::userIsChanged(){
    QDate year = ui->yearEdit->date();
    showAssets();
    yearDateChanged(year);
}

void AssetsViewer::showAssets()
{
    AssetsIO  mov(this) ;
    AssetModel * model = mov.getModelAssets();

    /** \todo move header naming into the model */
    model->setHeaderData(ASSETS_ACCOUNT_ID,Qt::Horizontal,trUtf8("Account id"));
    model->setHeaderData(ASSETS_LABEL,Qt::Horizontal,trUtf8("Label"));
    model->setHeaderData(ASSETS_DATE,Qt::Horizontal,trUtf8("Date"));
    model->setHeaderData(ASSETS_DURATION,Qt::Horizontal,trUtf8("Duration"));
    model->setHeaderData(ASSETS_MODE,Qt::Horizontal,trUtf8("Mode"));
    model->setHeaderData(ASSETS_VALUE,Qt::Horizontal,trUtf8("Value"));
    model->setHeaderData(ASSETS_YEARLY_RESULT,Qt::Horizontal,trUtf8("Yearly value"));
    model->setHeaderData(ASSETS_RESIDUAL_VALUE,Qt::Horizontal,trUtf8("Residual value"));
    model->setHeaderData(ASSETS_YEARS,Qt::Horizontal,trUtf8("Years to run"));
    model->setHeaderData(ASSETS_RATE,Qt::Horizontal,trUtf8("Rate"));
    model->setHeaderData(ASSETS_MOVEMENT,Qt::Horizontal,trUtf8("Movement"));
    model->setHeaderData(ASSETS_COMMENT,Qt::Horizontal,trUtf8("Comment"));
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setStretchLastSection ( true );
    ui->tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
    ui->tableView->setColumnHidden(ASSETS_ID,true);
    ui->tableView->setColumnHidden(ASSETS_USER_UID,true);
    ui->tableView->setColumnHidden(ASSETS_TAXEACTS,true);
    ui->tableView->setColumnHidden(ASSETS_TRACE,true);
}

void AssetsViewer::recordAsset(){
    QHash<int,QVariant>  hashValues;
    QHash<int,QVariant>  hashValuesMovements;
    QString bankName = ui->bankComboBox->currentText();
    QDate year = ui->yearEdit->date();
    
    AssetsManager manager;
    AssetsIO asIO(this);
    QString userUid = asIO.getUserUid();
    int accountId = 0;
    accountId = asIO.getIdFromBankName(bankName);
    QString label = ui->detailsEdit->text();
    QDate beginDate = ui->dateEdit->date();
    QString date = beginDate.toString("yyyy-MM-dd");
    QDate currDate = QDate::currentDate();
    QString currentDate = currDate.toString("yyyy-MM-dd");
    double duration = ui->durationDoubleSpinBox->value();
    QString choiceModeComboBox = ui->modeComboBox->currentText();
    int mode = 0;
    mode = manager.getHashForModeComboBox().key(choiceModeComboBox);
    double value = ui->valueDoubleSpinBox->value();
    double taxActs = 0.00;
    double yearlyValue = manager.getYearlyValue(date,currDate,value,mode,duration);
    double residualValue = manager.getCalculatedResidualValue(value,yearlyValue);
    QVariant movement = QVariant();
    QString comments = ui->commentsEdit->text();
    QVariant trace = QVariant();

    int avMovIdForAsset = -2;
    int type = SPENDING;
    QString labelMov = trUtf8("Fixed Asset");
    int validity = 0;
    int isValid = 1;
    QString traceMov = QString();
    QString commentMov = QString();
    double rate = manager.getRate(beginDate,duration,mode);
    hashValuesMovements = manager.getHashOfValuesMovements(//todo
                                         avMovIdForAsset ,
                                         userUid,
                                         accountId,//bank
                                         type,
                                         labelMov,
                                         date,
                                         date ,
                                         value ,
                                         commentMov,
                                         validity ,
                                         traceMov ,
                                         isValid ,
                                         comments);
    if (!asIO.insertIntoMovements(hashValuesMovements))  {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Asset is not recorded in movements."));
    }
    int movementLastId = asIO.getLastMovementId();
    if (movementLastId == -1) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Unable to get last movement id."));
    }
    movement = QVariant(movementLastId);                                        
    hashValues = manager.getHashOfValues(userUid,
                                         accountId,
                                         label,
                                         date,
                                         duration,
                                         mode,
                                         value,
                                         taxActs,
                                         yearlyValue,
                                         residualValue,
                                         int(duration),
                                         rate,
                                         movement,
                                         comments,
                                         trace);
    
    if (!asIO.insertIntoAssets(hashValues)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Asset not recorded."));
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Asset recorded."));
    }
    showAssets();
    yearDateChanged(year);
}

void AssetsViewer::deleteAsset(){
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if (!index.isValid()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Select a line."));
    }
    QString bankName = ui->bankComboBox->currentText();
    int row = index.row(); 
    AssetsIO  assetIO(this) ;
    int idMovement = assetIO.getMovementId(row);
    int idBank = assetIO.getIdFromBankName(bankName);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " idMovement =" << QString::number(idMovement) ;
    if (!assetIO.deleteAsset(row)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Asset is not deleted."));
    }
    if (!assetIO.deleteMovement(idMovement,idBank)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Movement of asset is not deleted."));
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Asset deleted."));
    }
    showAssets();
}

void AssetsViewer::fillModeComboBox() {
    AssetsManager manager;
    QStringList list = manager.getHashForModeComboBox().values();
    ui->modeComboBox->clear();
    ui->modeComboBox->addItems(list);
}

void AssetsViewer::refresh(){
    AssetsManager manager;
    manager.setRefreshedResidualValue();
}

void AssetsViewer::fillBankComboBox(){
    AssetsIO assIO(this);
    ui->bankComboBox->setModel(assIO.getBankComboBoxModel(this));
}

void AssetsViewer::yearDateChanged(const QDate & year){
    double sumOfYearlyValues = 0.00;
    QString yearString = year.toString("yyyy");
    AssetsManager manager;
    QStandardItemModel *model = new QStandardItemModel(this); 
    model = manager.getYearlyValues(year);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__)<< "model in viewer = "<< model->rowCount();
    for (int i = 0; i < model->rowCount(); i += 1)
    {
    	double yearlyValue = model->data(model->index(i,YEARLY_VALUE),Qt::DisplayRole).toDouble();
    	if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " yearlyValue =" << QString::number(yearlyValue) ;
    	sumOfYearlyValues += yearlyValue;
        }
    QString textLabel = "Total value to declare for "+yearString+" = "+QString::number(sumOfYearlyValues);
    ui->declareLabel->setText(textLabel);
}

void AssetsViewer::writeLabelByRow(const QModelIndex& index){
    int row = index.row();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in writeLabelByRow =" << QString::number(row) ;
    QString yearString = ui->yearEdit->date().toString("yyyy");
    AssetsManager manager;
    double yearlyValue = manager.getYearlyValue(yearString ,row);
    QString label = manager.getLabelFromRow(row);
    QString textLabel = QString("Value to declare for %1 and %2 = %3").arg(yearString,
                                                                           label,
                                                                           QString::number(yearlyValue));
    ui->declareLabel->setText(textLabel);    
}

void AssetsViewer::changeEvent(QEvent *e){
    QWidget::changeEvent(e);
    if (e->type()== QEvent::LanguageChange)
    {
    	  ui->retranslateUi(this);
    	  //fillBankComboBox();
    	  fillModeComboBox();
        }
}
