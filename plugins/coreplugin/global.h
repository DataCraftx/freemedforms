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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef COREGLOBAL_H
#define COREGLOBAL_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/licenseterms.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QDir;
class QFont;
class QDomElement;
class QWidget;
QT_END_NAMESPACE

#include <QFileInfo>
#include <QString>
#include <QHash>
#include <QMessageBox>

/**
 * \file global.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.18
 * \date 10 Aug 2009
*/

namespace Core {

namespace Constants {
const char* const TOKEN_OPEN   = "[";
const char* const TOKEN_CLOSE  = "]";
}

namespace tkGlobal {

enum Warn {
    WarnUser = 0,
    DontWarnUser
};


// Libs and OS informations
/** \brief Return true if the application was build in debug mode. */
CORE_EXPORT bool isDebugCompilation();
/** \brief Return true if the application was build using the SVN. */
CORE_EXPORT bool isSvnBuild();
/** \brief Return true if the application was build with all private parts. */
CORE_EXPORT bool isFullApplication();
/** \brief Return true if the application is running on MacOs. */
CORE_EXPORT bool isRunningOnMac();
/** \brief Return true if the application is running on Windows. */
CORE_EXPORT bool isRunningOnWin();
/** \brief Return true if the application is running on Linux. */
CORE_EXPORT bool isRunningOnLinux();
/** \brief Return true if the application is running on FreeBsd. */
CORE_EXPORT bool isRunningOnFreebsd();
/** \brief Returns informations of 'uname -a' command. Works on Linux, MacOs, FreeBSD */
CORE_EXPORT QString uname();
CORE_EXPORT QString osName();

// Widget geometry
/** \brief Center a widget into the screen. */
CORE_EXPORT void centerWidget( QWidget * win );
/** \brief Toggle a widget to fullScreen. */
CORE_EXPORT void setFullScreen( QWidget* win, bool on );

// Message Boxes and default dialogs
CORE_EXPORT void informativeMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
CORE_EXPORT void warningMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
CORE_EXPORT bool yesNoMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
CORE_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, const QStringList &buttonsText, const QString &title = QString::null );
CORE_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, QMessageBox::StandardButtons buts, QMessageBox::StandardButton defaultButton, const QString &title = QString::null );
CORE_EXPORT bool okCancelMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
CORE_EXPORT bool functionNotAvailableMessageBox( const QString &functionText );
CORE_EXPORT void quickDebugDialog( const QStringList &texts);
CORE_EXPORT bool defaultLicenceAgreementDialog(const QString &message, Core::LicenseTerms::AvailableLicense licence);

// Dialogs for user input
CORE_EXPORT QString askUser( const QString &title, const QString &question );

// Working with files and dirs
CORE_EXPORT bool removeDir(const QString &name, QString *error);
CORE_EXPORT QFileInfoList getFiles( QDir fromDir, const QStringList& filters, bool recursive = true );
CORE_EXPORT QFileInfoList getFiles( QDir fromDir, const QString& filters = QString::null, bool recursive = true );
CORE_EXPORT bool checkDir( const QString & absPath, bool createIfNotExist, const QString & logDirName = QString::null );
CORE_EXPORT bool saveStringToFile( const QString &toSave, const QString &toFile, const Warn warnUser = WarnUser, QWidget *parent=0 );
CORE_EXPORT bool saveStringToFile( const QString &toSave,  const QString &dirPath, const QString &filters, QWidget *parent = 0 );
CORE_EXPORT QString readTextFile( const QString &toRead, const Warn warnUser = WarnUser, QWidget *parent = 0 );
CORE_EXPORT QString isDirExists(const QString &absPath);
CORE_EXPORT QString isFileExists(const QString &absPath);

// Some usefull widgets specific functions
/** \brief Toggle the echomode of a QLineEdit. */
CORE_EXPORT void switchEchoMode( QLineEdit * l );

// Some functions for HTML work
CORE_EXPORT QString fontToHtml( const QFont &font, const QColor &color = QColor("black") );
CORE_EXPORT QString textAlignmentToHtml( const Qt::Alignment &align );
CORE_EXPORT QString toHtmlAccent( const QString &html );

// Some functions for XML Handling
CORE_EXPORT QString createXml( const QString &mainTag, const QHash<QString,QString> &datas, const int indent = 0, const bool valueToBase64 = false );
CORE_EXPORT bool    readXml( const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readDatas,const bool valueFromBase64 = false );
CORE_EXPORT QString xmlRead(const QDomElement &father, const QString &name, const QString &defaultValue);
CORE_EXPORT QString xmlRead(const QDomElement &father, const QString &name, const char *defaultValue);
CORE_EXPORT int     xmlRead(const QDomElement &father, const QString &name, const int defaultValue);
CORE_EXPORT int     xmlRead(const QDomElement &father, const QString &name, const long int defaultValue);
CORE_EXPORT bool    xmlRead(const QDomElement &father, const QString &name, const bool defaultValue);
CORE_EXPORT void    xmlWrite(QDomElement &father, const QString &name, const QString &value);
CORE_EXPORT void    xmlWrite(QDomElement &father, const QString &name, char *value);
CORE_EXPORT void    xmlWrite(QDomElement &father, const QString &name, int value);
CORE_EXPORT void    xmlWrite(QDomElement &father, const QString &name, long int value);
CORE_EXPORT void    xmlWrite(QDomElement &father, const QString &name, bool value);

// Some functions for token replacement [ text added if token exists [TOKEN] text added if token exists ]
CORE_EXPORT int replaceToken( QString &textToAnalyse, const QString &token, const QString &value );

}  // end tkGlobal

}  // end Core

#endif  // COREGLOBAL_H
