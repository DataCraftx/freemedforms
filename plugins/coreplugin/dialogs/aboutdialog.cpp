/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/**
  \class AboutDialog
   \brief This dialog shows a complete About dialog with a tabwidget containing : a short about text, a complete team list, a full licence text.
   It also shows the compilation date and time and the actual version of the application. \n
   The name of the application is picked into qApp->applicationName() and the version into qApp->applicationVersion(). \n
   You can inform the team using the addTeamList().\n
   You can get the default licence terms using the static getLicenceTerms() function. This function manages english and french language by looking the default QLocale()::name().\n
  \ingroup toolkit
  \ingroup widget_toolkit
  \todo add LGPLfrench, GPLv3french licence terms.
*/

#include "aboutdialog.h"
#include "ui_AboutDialog.h"
#include <coreplugin/iaboutpage.h>

#include <extensionsystem/pluginmanager.h>

#include <QStackedLayout>

Q_DECLARE_METATYPE(Core::IAboutPage*)

static inline Core::IAboutPage *pageOfItem(const QTreeWidgetItem *item = 0)
{
    if (!item)
        return 0;
    return qVariantValue<Core::IAboutPage*>(item->data(0, Qt::UserRole));
}

using namespace Core;
using namespace Core::Internal;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Core::Internal::Ui::AboutDialog)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;
    m_ui->setupUi(this);
    m_slayout = new QStackedLayout(m_ui->forStack);
    m_slayout->setMargin(0);
    m_slayout->setSpacing(0);
    m_ui->applicationNameLabel->setText(qApp->applicationName());
    setWindowTitle( qApp->applicationName() );
    setObjectName( "AboutDialog" );

    m_ui->tree->header()->hide();
    connect(m_ui->tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(currentItemChanged(QTreeWidgetItem*)));

    QList<IAboutPage*> pages = ExtensionSystem::PluginManager::instance()->getObjects<IAboutPage>();
    setPages(pages);
//    m_ui->splitter->setStretchFactor(0,1);
//    m_ui->splitter->setStretchFactor(1,3);
    this->adjustSize();
}

void AboutDialog::setPages(const QList<IAboutPage*> pages)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;

    CategoryItemMap categories;
    QVariant pagePtr;

    m_ui->tree->clear();
    foreach (IAboutPage *page, pages) {
        // ensure category root
        const QString categoryName = page->category();
        CategoryItemMap::iterator cit = categories.find(categoryName);
        if (cit == categories.end()) {
            QTreeWidgetItem *categoryItem = new QTreeWidgetItem(m_ui->tree);
            categoryItem->setFlags(Qt::ItemIsEnabled);
            categoryItem->setText(0, page->category());
            qVariantSetValue<IAboutPage*>(pagePtr, 0);
            categoryItem->setData(0, Qt::UserRole, pagePtr);
            cit = categories.insert(categoryName, categoryItem);
        }
        // add item
        QTreeWidgetItem *pageItem = new QTreeWidgetItem(cit.value(), QStringList(page->name()));
//        pageItem->setIcon(0, page->icon());
        qVariantSetValue<IAboutPage*>(pagePtr, page);
        pageItem->setData(0, Qt::UserRole, pagePtr);
        pageItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        m_slayout->addWidget(page->widget());
        page->refreshContents();
    }
    m_ui->tree->sortItems(0,Qt::AscendingOrder);
}

void AboutDialog::showDialog()
{
    m_ui->tree->expandAll();
    if (QTreeWidgetItem *rootItem = m_ui->tree->topLevelItem(0)) {
        m_ui->tree->scrollToItem(rootItem);
        if (rootItem->childCount())
            m_ui->tree->setCurrentItem(rootItem->child(0));
    }
    exec();
}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}

void AboutDialog::currentItemChanged(QTreeWidgetItem *cat)
{
    if (IAboutPage *page = pageOfItem(cat)) {
        m_slayout->setCurrentWidget(page->widget());
    }
}


