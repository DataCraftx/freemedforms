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
#ifndef IFORMITEM_H
#define IFORMITEM_H

#include <coreplugin/idebugpage.h>

#include <formmanagerplugin/formmanager_exporter.h>
#include <formmanagerplugin/iformitemspec.h>
#include <formmanagerplugin/iformitemscripts.h>
#include <formmanagerplugin/iformitemvalues.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QWidget>
#include <QVariant>
#include <QPointer>
#include <QHash>

#include <QDebug>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

/**
 * \file iformitem.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Jan 2012
*/

namespace Form {
class IFormWidget;
class IFormItemData;
class FormItemIdentifiants;
class FormPage;
class FormMain;
class IFormIO;
class FormMainDebugPage;

class FORM_EXPORT FormItem : public Form::FormItemIdentifiants
{
    Q_OBJECT
//    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    FormItem(QObject *parent=0) :
            FormItemIdentifiants(parent),
            m_Spec(new FormItemSpec),
            m_Scripts(new FormItemScripts),
            m_Values(new FormItemValues(this)),
            m_FormWidget(0),
            m_ItemDatas(0),
            m_PatientData(-1)
            {}

    virtual ~FormItem();
    Form::FormItem *parentFormItem() const;
    Form::FormMain *parentFormMain() const;

    Form::FormItemSpec *spec() const {return m_Spec;}
    Form::FormItemScripts *scripts() const {return m_Scripts;}
    Form::FormItemValues *valueReferences() const {return m_Values;}

    // Access to database values. Pointer will not be deleted
    void setItemDatas(Form::IFormItemData *data) {m_ItemDatas = data;}
    Form::IFormItemData *itemDatas() {return m_ItemDatas;}

    // Access to the user's widget
    virtual void setFormWidget(Form::IFormWidget *w) {m_FormWidget=w;}
    virtual IFormWidget *formWidget() {return m_FormWidget;}

    // Access to the FormItem tree
    virtual FormMain *createChildForm(const QString &uuid = QString::null) {Q_UNUSED(uuid); return 0;}
    virtual FormItem *createChildItem(const QString &uuid = QString::null);
    virtual FormPage *createPage(const QString &uuid = QString::null) {Q_UNUSED(uuid); return 0;}
    virtual QList<FormItem *> formItemChildren() const;
    virtual QList<FormItem *> flattenFormItemChildren() const;

    // FormIO extra datas
    virtual void addExtraData(const QString &id, const QString &data);
    virtual QHash<QString,QString> extraDatas() const {return m_ExtraDatas;}
    virtual void clearExtraDatas() {m_ExtraDatas.clear();}
    virtual QStringList getOptions() const;

    // Data representation
    virtual void setPatientDataRepresentation(const int ref) {m_PatientData = ref;}
    virtual int patientDataRepresentation() const {return m_PatientData;}

public Q_SLOTS:
    virtual void languageChanged();

private:
    Form::FormItemSpec *m_Spec;
    Form::FormItemScripts *m_Scripts;
    Form::FormItemValues *m_Values;
    Form::IFormWidget *m_FormWidget;
    Form::IFormItemData *m_ItemDatas;
    QHash<QString, QString> m_ExtraDatas;
    int m_PatientData;
};
/** Returns the first level Form::FormItem children. */
inline QList<Form::FormItem*> Form::FormItem::formItemChildren() const
{
     QList<Form::FormItem*> list;
     foreach(QObject *o, children()) {
          Form::FormItem *i = qobject_cast<Form::FormItem*>(o);
          if (i) {
              list.append(i);
          }
     }
     return list;
}
/** Returns all level Form::FormItem children. */
inline QList<FormItem *> Form::FormItem::flattenFormItemChildren() const
{
    QList<Form::FormItem *> list;
    foreach(QObject *o, children()) {
         Form::FormItem *i = qobject_cast<Form::FormItem*>(o);
         if (i) {
             list.append(i);
             list.append(i->flattenFormItemChildren());
         }
    }
    return list;
}
/** Returns the Form::FormItem parent. */
inline Form::FormItem *Form::FormItem::parentFormItem() const
{
    return qobject_cast<Form::FormItem *>(parent());
}
/** Returns the Form::FormMain parent. */
inline Form::FormMain *Form::FormItem::parentFormMain() const
{
    QObject *o = this->parent();
    while (o) {
        FormMain *f = qobject_cast<Form::FormMain *>(o);
        if (f)
            return f;
        o = o->parent();
    }
    return 0;
}


class FORM_EXPORT FormPage : public FormItem
{
    Q_OBJECT
public:
    FormPage(QObject *parent=0) :
            FormItem(parent) {}
    ~FormPage() {}

    virtual FormPage *createPage(const QString &uuid = QString::null) {Q_UNUSED(uuid); return 0;}

    virtual void languageChanged();

private:
};

class FORM_EXPORT FormMain : public FormItem
{
    Q_OBJECT
public:
    enum EpisodeCountPossibilities {
        NoEpisode = 0,
        UniqueEpisode,
        MultiEpisode
    };

    FormMain(QObject *parent = 0);
    ~FormMain();

    void setIoFormReader(IFormIO *reader);
    IFormIO *reader() const;

    bool isEmptyRootForm() const {return (m_Reader!=0);}

    void setModeUniqueName(const QString &modeUid) {m_ModeName = modeUid;}
    QString modeUniqueName() const {return m_ModeName;}

    void setUseNameAsNSForSubItems(const bool b) {m_UseNameAsNSForSubItems=b;}
    bool useNameAsNSForSubItems() const {return m_UseNameAsNSForSubItems;}

    FormMain *createChildForm(const QString &uuid = QString::null);
    FormPage *createPage(const QString &uuid = QString::null);

    virtual void languageChanged();
    void clear();

    virtual FormMain *formParent() const;
    virtual FormMain *rootFormParent() const;
    virtual QList<FormMain *> flattenFormMainChildren() const;
    virtual QList<FormMain *> firstLevelFormMainChildren() const;
    virtual FormMain *formMainChild(const QString &uuid) const;

    virtual void setEpisodePossibilities(const int i) {m_Episode = i;}
    virtual int episodePossibilities() const {return m_Episode;}

    QString printableHtml(bool withValues = true) const;

    // Specific to FormMain
    void createDebugPage();
    void toTreeWidget(QTreeWidget *tree);

    void emitFormLoaded();

Q_SIGNALS:
    void formLoaded();
//    void itemAdded(const QString &uuid);
//    void subFormAdded(const QString &uuid);

private:
    Form::IFormIO *m_Reader;
    FormMainDebugPage *m_DebugPage;
    int m_Episode;
    QString m_ModeName;
    QList<QPixmap> m_Shots;
    bool m_UseNameAsNSForSubItems;
};
inline Form::FormMain *Form::FormMain::formParent() const
{
    return qobject_cast<FormMain*>(parent());
}
/** Returns the Empty Root Form parent of the Form::FormMain. */
inline Form::FormMain *Form::FormMain::rootFormParent() const
{
    Form::FormMain *parent = (Form::FormMain*)this;
    if (parent->isEmptyRootForm())
        return parent;
    Form::FormMain *newparent = 0;
    while ((newparent = formParent()) && !newparent->isEmptyRootForm()) {
        parent = newparent;
    }
    return (Form::FormMain*)parent;
}
/** Returns all Form::FormMain children of the Form::FormMain  (all level). */
inline QList<Form::FormMain *> Form::FormMain::flattenFormMainChildren() const
{
     QList<Form::FormMain *> list;
     foreach(QObject *o, children()) {
          Form::FormMain *i = qobject_cast<Form::FormMain*>(o);
          if (i) {
              list.append(i);
              list.append(i->flattenFormMainChildren());
          }
     }
     return list;
}
/** Returns all Form::FormMain first level children of the Form::FormMain. */
inline QList<FormMain *> Form::FormMain::firstLevelFormMainChildren() const
{
    QList<Form::FormMain *> list;
    foreach(QObject *o, children()) {
         Form::FormMain *i = qobject_cast<Form::FormMain*>(o);
         if (i) {
             list.append(i);
         }
    }
    return list;
}
/** Returns the Form::FormMain child with the uuid \e uuid. */
inline Form::FormMain *Form::FormMain::formMainChild(const QString &uuid) const
{
    QList<FormMain *> forms = flattenFormMainChildren();
    for(int i=0; i<forms.count(); ++i) {
        FormMain *form = forms.at(i);
        if (form->uuid()==uuid)
            return form;
    }
    return 0;
}

class FORM_EXPORT FormMainDebugPage : public Core::IDebugPage
{
    Q_OBJECT
public:
    FormMainDebugPage(FormMain *form, QObject *parent);
    ~FormMainDebugPage();

    QString id() const       { return objectName();   }
    QString name() const;
    QString category() const;

    void refreshContents();

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
    FormMain *m_Form;
};

} // end Form

#endif // IFORMITEM_H
