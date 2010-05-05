/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
/***************************************************************************
 *   Main Developper : Guillaume DENRY <guillaume.denry@gmail.com>         *
 *   Contributors :                                                        *
 *       Eric MAEKER <eric.maeker@free.fr>                                 *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef XMLFORMIO_H
#define XMLFORMIO_H

#include <formmanagerplugin/iformio.h>

#include <QDomNode>
#include <QObject>

namespace Form {
class FormItem;
class FormMain;
class IFormWidgetFactory;
}

namespace XmlForms {

//namespace Internal {
//class XmlIOPrivate;
//} // End Internal

class XmlFormIO : public Form::IFormIO
{
     Q_OBJECT
public:
    XmlFormIO(const QString &absFileName, QObject *parent=0);
    ~XmlFormIO();

    QStringList fileFilters() const;

    QString managedFileExtension() const { return QString("xml"); }
    bool canReadFile() const;
    bool setFileName(const QString &absFileName);

    bool loadForm();
    bool saveForm(QObject *treeRoot) { return true; }

    static QString lastestXmlVersion();

private:
    bool loadForm(const QString &file, Form::FormMain *rootForm);
    bool loadElement(Form::FormItem *item, QDomElement &rootElement);
    bool createElement(Form::FormItem *item, QDomElement &element);
    bool createItemWidget(Form::FormItem *item, QWidget *parent = 0);
    bool createFormWidget(Form::FormMain *form);
    bool createWidgets();

private:
     mutable QString m_AbsFileName;
     Form::FormMain *m_ActualForm;
};

}  // End XmlForms

#endif  //  XMLFORMIO_H
