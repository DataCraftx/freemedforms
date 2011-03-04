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
 ***************************************************************************/
#ifndef FREEPAD_MAINWINDOW_H
#define FREEPAD_MAINWINDOW_H

#include <mainwindowplugin/mainwindow_exporter.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ipadtools.h>

#include <utils/global.h>

#include <QCloseEvent>
#include <QComboBox>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.1.0
 * \date 03 Jan 2011
*/

namespace MainWin {
namespace Internal {
class MainWinPrivate;

namespace Ui {
class MainWindow;
}  // End Ui
}  // End Internal

class FREEPAD_MAINWIN_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void createDockWindows();
    void readSettings();
    void writeSettings();
    void createStatusBar();
    void changeFontTo(const QFont &font);


public Q_SLOTS:
    void postCoreInitialization();
//    void refreshPatient();
    void updateIconBadgeOnMacOs();

    // Interface of MainWidowActionHandler
    bool newFile();
    bool openFile();
    void readFile(const QString &file);
    bool saveFile();
    bool saveAsFile();

    bool applicationPreferences();

    void updateCheckerEnd();

    void aboutToShowRecentFiles();
    void openRecentFile();

private slots:
	void padTextChanged();

protected:
    void closeEvent( QCloseEvent *event );
    void changeEvent(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

public:
    Internal::Ui::MainWindow *m_ui;
    Internal::MainWinPrivate *d;

private:
	QMap<QString,QVariant> m_tokens;
	Core::IPadTools *m_padTools;
};

} // End Core

#endif  // FREEPAD_MAINWINDOW_H
