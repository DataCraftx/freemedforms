/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFDRUGSWIDGET_H
#define MFDRUGSWIDGET_H

// include drugswidget headers
class mfDrugsModel;

// include toolkit headers
#include <QButtonLineEdit.h>
#include <tkSettings.h>

// include Qt headers
#include <QWidget>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

#ifndef DRUGS_INTERACTIONS_STANDALONE
#include <mfObject.h>
#include <mfFormWidgetInterface.h>
#include <mfAbstractWidget.h>
#endif


/**
 * \file mfDrugsWidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.9
 * \date 07 Jun 2009
*/

#ifndef DRUGS_INTERACTIONS_STANDALONE
class mfDrugsWidgetPlugin : public mfFormWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES( mfBaseInterface mfFormWidgetInterface );

public:
    mfDrugsWidgetPlugin();

    QStringList   widgets() const;
    bool          isContainer( const int idInStringList ) const;
    mfAbstractWidget * getWidget( mfObject * mfo, mfAbstractWidget * parent = 0 ) const;
};
#endif

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfDrugsWidget implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfDrugsWidget : public mfAbstractWidget
{
    Q_OBJECT
public:
    mfDrugsWidget( mfObject * mfo, mfAbstractWidget * parent );
    ~mfDrugsWidget();

private Q_SLOTS:
    void updateObject( int val );
    void updateWidget();
    void retranslateUi( const QString & );
    void createDefaultSettings( tkSettings *settings );

private:
    void createConnections();

private:
    mfDrugsModel         *m_PrescriptionModel;
    QString               m_iniPath;
    bool                  m_WithPrescribing, m_WithPrinting;
};

#endif
