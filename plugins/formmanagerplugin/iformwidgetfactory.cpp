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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "iformwidgetfactory.h"
#include "iformitem.h"

/**
  \class IFormWidgetFactory
 This class is an interface for the form's widget. Object need to be created by plugins and added to
 the plugins manager objects pool.
*/

#include <QLocale>
#include <QEvent>
#include <QBoxLayout>

using namespace Form;


IFormWidget::IFormWidget(Form::FormItem *formItem, QWidget *parent )
        : QWidget(parent), m_FormItem(formItem)
{
}

void IFormWidget::createLabel(const QString &text, Qt::Alignment horizAlign)
{
    m_Label = new QLabel(this);
    QLabel *l = m_Label;
    l->setFrameStyle( IFormWidget::ItemLabelFrame );
    l->setText( text );
    l->setAlignment( Qt::AlignTop | horizAlign );
    l->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );
    l->setWordWrap(true);
}

QBoxLayout *IFormWidget::getBoxLayout(const int labelOption, const QString &text, QWidget *parent)
{
    QBoxLayout * hb;
    m_Label = new QLabel(this);
    if (labelOption == Label_OnTop) {
        hb = new QBoxLayout(QBoxLayout::TopToBottom, parent );
        createLabel(text, Qt::AlignTop | Qt::AlignLeft );
        hb->setSpacing( 0 );
    } else {
        /** \todo Code specific label options */
        hb = new QBoxLayout( QBoxLayout::LeftToRight, parent );
        if (labelOption != Label_NoLabel) {
            createLabel(text, Qt::AlignTop | Qt::AlignRight );
        }
    }
    hb->setSpacing(5);
    hb->setMargin(5);
    return hb;
}


void IFormWidget::changeEvent(QEvent * event)
{
    QString loc = QLocale().name().left(2);
    if ((event->type() == QEvent::LanguageChange) &&
        (m_OldTrans != loc)) {
        m_OldTrans = loc;
        retranslate();
        event->accept();
    }
    else
        QWidget::changeEvent( event );
}
