/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef IFORMWIDGETFACTORY_H
#define IFORMWIDGETFACTORY_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>

namespace Form {

class IFormWidget;
class FormItem;

// This part is very closed to QDesignerCustomWidgetInterface so the translation should be easier
class FORM_EXPORT IFormWidgetFactory : public QObject
{
    Q_OBJECT
public:
    IFormWidgetFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFormWidgetFactory() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual bool extensionInitialized() = 0;
    virtual QStringList providedWidgets() const = 0;
    virtual bool isContainer( const int idInStringList ) const = 0;
    inline bool isContainer(const QString &name) {return isContainer(providedWidgets().indexOf(name)); }
    virtual bool isInitialized() const = 0;
    virtual IFormWidget *createWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent = 0) = 0;
};


class FORM_EXPORT IFormWidget : public QWidget
{
    Q_OBJECT
public:
    enum QFrameStyleForLabels {
        FormLabelFrame = QFrame::Panel | QFrame::Sunken,
        ItemLabelFrame = QFrame::NoFrame,
        HelpTextFrame =  QFrame::Panel | QFrame::Sunken
    };

    enum LabelOptions {
        Label_NoOptions = 0,
        Label_NoLabel,
        Label_OnTop,
        Label_OnBottom,
        Label_OnLeft,
        Label_OnRigth,
        Label_OnTopCentered
    };

    IFormWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    virtual ~IFormWidget() {}

    virtual void addWidgetToContainer(IFormWidget *) {}
    virtual bool isContainer() const {return false;}

    virtual void createLabel(const QString &text, Qt::Alignment horizAlign);
    virtual QBoxLayout *getBoxLayout(const int labelOption, const QString &text, QWidget *parent);

    virtual void changeEvent(QEvent *event);

    virtual void setFormItem(Form::FormItem *link) { m_FormItem = link; }
    virtual Form::FormItem *formItem() { return m_FormItem; }

public Q_SLOTS:
    virtual void retranslate() {}

public:
    QLabel *m_Label;
    Form::FormItem *m_FormItem;
    QString m_OldTrans;
};

} // namespace Form

#endif // IFORMWIDGETFACTORY_H
