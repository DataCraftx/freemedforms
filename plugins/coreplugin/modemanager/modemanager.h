/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/
/***************************************************************************
 *   Adaptation to FreeMedForms : Eric MAEKER, <eric.maeker@gmail.com>     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include <coreplugin/core_exporter.h>
#include <coreplugin/imainwindow.h>

QT_BEGIN_NAMESPACE
class QSignalMapper;
class QMenu;
QT_END_NAMESPACE

namespace Utils {
class FancyTabWidget;
class FancyActionBar;
}

namespace Core {
class Command;
class IMode;
class IMainWindow;

class CORE_EXPORT ModeManager : public QObject
{
    Q_OBJECT

public:
    ModeManager(IMainWindow *mainWindow);

    void init(Utils::FancyTabWidget *modeStack);
    static ModeManager *instance() { return m_instance; }

    IMode* currentMode() const;
    IMode* mode(const QString &id) const;

    void addAction(Command *command, int priority, QMenu *menu = 0);
    void addWidget(QWidget *widget);

signals:
    void currentModeAboutToChange(Core::IMode *mode);
    void currentModeChanged(Core::IMode *mode);

public slots:
    void activateMode(const QString &id);
    void setFocusToCurrentMode();

private slots:
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);
    void currentTabAboutToChange(int index);
    void currentTabChanged(int index);
    void updateModeToolTip();
    void languageChanged();

private:
    int indexOf(const QString &id) const;

    static ModeManager *m_instance;
    IMainWindow *m_mainWindow;
    Utils::FancyTabWidget *m_modeStack;
    Utils::FancyActionBar *m_actionBar;
    QMap<Command*, int> m_actions;
    QVector<IMode*> m_modes;
    QVector<Command*> m_modeShortcuts;
    QSignalMapper *m_signalMapper;
    QList<int> m_addedContexts;
};

} // namespace Core

#endif // MODEMANAGER_H
