/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef EDITORCONTEXT_H
#define EDITORCONTEXT_H

// include toolkit headers
#include <texteditorplugin/texteditor_exporter.h>
#include <coreplugin/contextmanager/icontext.h>


namespace Editor {
class TextEditor;

namespace Internal {

class EditorContext : public Core::IContext
{
public:
    EditorContext(TextEditor *parent) : Core::IContext(parent), w(parent)
    {
        setObjectName("EditorContext");
    }

    void addContext( int uid )
    {
        if (!m_Context.contains(uid))
            m_Context.append(uid);
    }
    void clearContext() { m_Context.clear(); }

    QList<int> context() const { return m_Context; }
    QWidget *widget()          { return w; }

private:
    TextEditor *w;
    QList<int> m_Context;
};

}  // End Internal
}  // End Editor

#endif // EDITORCONTEXT_H
