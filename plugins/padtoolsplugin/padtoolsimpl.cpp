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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
  \class PadTools::PadToolsImpl
  Implementation of the virtual Core::IPadTools, start of everything.
 */

#include "padtoolsimpl.h"
#include "tokenpool.h"
#include "pad_analyzer.h"
#include "pad_highlighter.h"

using namespace PadTools;

PadToolsImpl::PadToolsImpl(QObject *parent) :
    Core::IPadTools(parent)
{
    _pool = new TokenPool(this);
}

PadToolsImpl::~PadToolsImpl()
{}

Core::ITokenPool *PadToolsImpl::tokenPool() const
{
    return _pool;
}


/** Analyse a string \e templ for \e tokens, manages a list of \e errors (output) and returns the parsed string.*/
QString PadToolsImpl::parse(const QString &templ, QMap<QString,QVariant> &tokens, QList<Core::PadAnalyzerError> &errors)
{
    Q_UNUSED(tokens); //TMP

    PadAnalyzer analyzer;
    QString t = templ;
    if (t.contains("&lt;")) {
        t = t.replace("&lt;","<").replace("&gt;",">");
    }
    PadDocument *pad = analyzer.analyze(t);
    errors = analyzer.lastErrors();

    pad->run(tokens);
    return pad->outputDocument()->toHtml();
}

/** Creates a syntax highlighter for the \e textEdit usng the \e tokens.*/
QSyntaxHighlighter *PadToolsImpl::createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens)
{
    Q_UNUSED(tokens); //TMP
    return new PadHighlighter(textEdit);
}
