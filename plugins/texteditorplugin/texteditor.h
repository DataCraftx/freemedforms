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
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@gmail.com                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <texteditorplugin/texteditor_exporter.h>
#include <texteditorplugin/tableeditor.h>

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QFocusEvent>
class QMenu;

/**
 * \file texteditor.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

namespace Editor {

namespace Internal {
class TextEditorPrivate;
class EditorActionHandler;
}

class EDITOR_EXPORT TextEditor : public TableEditor
{
    friend class Internal::TextEditorPrivate;
    friend class Internal::EditorActionHandler;
    Q_OBJECT
    Q_PROPERTY( QString html READ getHtml WRITE setHtml USER true)

public:
    enum Type
    {
        Simple            = 0x01,
        WithTables        = 0x02,
        WithIO            = 0x04,
        WithTextCompleter = 0x08,
        Full       = Simple | WithTables | WithIO | WithTextCompleter
    };
    Q_DECLARE_FLAGS(Types, Type)

    TextEditor(QWidget *parent = 0, Types type = Simple);
    ~TextEditor();

    virtual QTextEdit *textEdit() const;

    QString getHtml()                        { return textEdit()->toHtml(); }
    void    setHtml(const QString & html)    { textEdit()->setHtml( html ); }
    void    setTypes(Types type);

public Q_SLOTS:
    virtual void toogleToolbar(bool state);

protected Q_SLOTS:
    void fileOpen();
    void saveAs();

//    void undo();
//    void redo();
//    void copy();
//    void paste();
//    void cut();
//    void selectall();
//    void clipboardDataChanged();

    void fontBigger();
    void fontSmaller();
    void textBold( bool checked );
    void textUnderline( bool checked );
    void textItalic( bool checked );
    void textStrike( bool checked );
    void textColor();
    void fontFormat();

    void addDate();
    void addUserName();
    void addPatientName();

    virtual void contextMenu(const QPoint &pos);

protected:
    virtual QMenu *getContextMenu();
    virtual bool toolbarIsVisible() const;
    virtual void hideToolbar();

private:
    Internal::TextEditorPrivate *d;

};

}  // End Editor

Q_DECLARE_OPERATORS_FOR_FLAGS(Editor::TextEditor::Types)

#endif // TEXTEDITOR_H
