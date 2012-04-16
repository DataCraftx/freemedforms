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
  \class PadTools::PadWriter
*/

#include "padwriter.h"
#include "tokenmodel.h"
#include "constants.h"
#include "pad_analyzer.h"
#include "pad_highlighter.h"

#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/itheme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <utils/log.h>

#include <QTimer>

#include "ui_padwriter.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;

static inline Core::UniqueIDManager *uid() { return Core::ICore::instance()->uniqueIDManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

namespace PadTools {
namespace Internal {
class PadWriterPrivate
{
public:
    PadWriterPrivate(PadWriter *parent) :
        ui(0),
        _tokenModel(0),
        _pad(0),
        _followedItem(0),
        _cursorHighlighTimer(0),
        q(parent)
    {
        _followedCharFormat.setUnderlineColor(QColor(Qt::cyan));
        _followedCharFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

        _cursorHighlighTimer = new QTimer(q);
        _cursorHighlighTimer->setSingleShot(true);
    }

    void createActions(QToolButton *button)
    {
        // Add options action
        QList<int> context = QList<int>() << Core::Constants::C_GLOBAL_ID;
        QAction *a;
        Core::Command *cmd;

        aFindCursor = a = new QAction(q);
        a->setObjectName("PadWriter.aFindCursor");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, a->objectName(), context);
        cmd->setTranslations(Constants::FIND_CURSOR_IN_RESULT, Constants::FIND_CURSOR_IN_RESULT, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        aAutoUpdate = a = new QAction(q);
        a->setObjectName("PadWriter.aAutoUpdate");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, a->objectName(), context);
        cmd->setTranslations(Constants::AUTO_UPDATE_RESULT, Constants::AUTO_UPDATE_RESULT, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        aSetDefaultValues = a = new QAction(q);
        a->setObjectName("PadWriter.aSetDefaultValues");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, a->objectName(), context);
        cmd->setTranslations(Constants::SET_TEST_VALUE_TO_TOKENS, Constants::SET_TEST_VALUE_TO_TOKENS, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());
    }

public:
    Ui::PadWriter *ui;
    TokenModel *_tokenModel;
    QAction *aFindCursor, *aAutoUpdate, *aSetDefaultValues;
    QAction *aTest1, *aTest2, *aTest3, *aTest4, *aTest5; // actions used to test different rawsource scenari
    PadDocument *_pad;
    PadFragment *_followedItem; // should not be deleted
    QList<QTextCharFormat> m_LastHoveredItemCharFormats, _followedItemCharFormats;
    QTextCharFormat _followedCharFormat;
    QTimer *_cursorHighlighTimer;

private:
    PadWriter *q;
};
}  // Internal
}  // PadTools

PadWriter::PadWriter(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PadWriterPrivate(this))
{
    d->ui = new Internal::Ui::PadWriter;
    d->ui->setupUi(this);

    // Create TokenModel
    d->_tokenModel = new TokenModel(this);
    d->ui->treeView->setModel(d->_tokenModel);
    d->ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);

    // Create PadDocument object
    d->_pad = new PadDocument();
    d->ui->wysiwyg->setPadDocument(d->_pad);
    connect(d->_pad, SIGNAL(padFragmentChanged(PadFragment*)), this, SLOT(onPadFragmentChanged(PadFragment*)));

    // Create PadHighlighter
//    PadHighlighter *highlight = new PadHighlighter(d->ui->rawSource->textEdit());

    // Add options action
    d->createActions(d->ui->optionsButton);
    connect(d->aFindCursor, SIGNAL(triggered()), this, SLOT(highlightCursor()));
    connect(d->aAutoUpdate, SIGNAL(triggered(bool)), this, SLOT(setAutoUpdateOfResult(bool)));
    connect(d->aSetDefaultValues, SIGNAL(triggered(bool)), this, SLOT(setTestValues(bool)));
    connect(d->ui->viewResult, SIGNAL(clicked()), this, SLOT(analyseRawSource()));
    connect(d->ui->viewError, SIGNAL(clicked()), this, SLOT(viewErrors()));
    connect(d->ui->findCursor, SIGNAL(clicked()), this, SLOT(highlightCursor()));
    connect(d->ui->outputToRaw, SIGNAL(clicked()), this, SLOT(outputToRaw()));

    // TEST
    QAction *a;

    d->ui->nextBlock->setText(tr("Next block"));
    connect(d->ui->nextBlock, SIGNAL(clicked()), this, SLOT(highLightNextBlock()));
    d->ui->prevBlock->setText(tr("Previous block"));
    connect(d->ui->prevBlock, SIGNAL(clicked()), this, SLOT(highLightPreviousBlock()));

    a = d->aTest1 = new QAction(this);
    a->setText("Tokens and strings");
    d->ui->scenari->addAction(a);

    a = d->aTest2 = new QAction(this);
    a->setText("Simple nested tokens & strings");
    d->ui->scenari->addAction(a);

    a = d->aTest3 = new QAction(this);
    a->setText("Multinested tokens & strings");
    d->ui->scenari->addAction(a);

    a = d->aTest4 = new QAction(this);
    a->setText("Tokens in table");
    d->ui->scenari->addAction(a);

    a = d->aTest5 = new QAction(this);
    a->setText("Multinested tokens in table");
    d->ui->scenari->addAction(a);

    connect(d->ui->scenari, SIGNAL(triggered(QAction*)), this, SLOT(changeRawSourceScenario(QAction*)));
    d->ui->scenari->setDefaultAction(d->aTest1);
    d->aTest1->trigger();

//    connect(d->ui->wysiwyg->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(wysiwygCursorChanged()));
//    connect(d->ui->rawSource->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(rawSourceCursorChanged()));
    d->ui->wysiwyg->textEdit()->setCursorWidth(3);
    d->ui->rawSource->textEdit()->setCursorWidth(3);
    // END TEST
}

PadWriter::~PadWriter()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void PadWriter::wysiwygCursorChanged()
{
    QTextCursor c = d->ui->wysiwyg->textEdit()->textCursor();
    d->ui->rawPos->setValue(d->_pad->positionTranslator().outputToRaw(c.position()));
    d->ui->ouputPos->setValue(c.position());

    QTextCursor raw(d->ui->rawSource->document());
    raw.setPosition(d->_pad->positionTranslator().outputToRaw(c.position()));
    raw.setPosition(raw.position() + 1, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setBackground(QBrush(QColor(Qt::lightGray)));
    raw.mergeCharFormat(format);
}

void PadWriter::rawSourceCursorChanged()
{
    QTextCursor c = d->ui->rawSource->textEdit()->textCursor();
    d->ui->rawPos->setValue(c.position());
    d->ui->ouputPos->setValue(d->_pad->positionTranslator().rawToOutput(c.position()));

    QTextCursor out(d->ui->wysiwyg->document());
    out.setPosition(d->_pad->positionTranslator().rawToOutput(c.position()));
    out.setPosition(out.position() + 1, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setBackground(QBrush(QColor(Qt::lightGray)));
    out.mergeCharFormat(format);
}

void PadWriter::changeRawSourceScenario(QAction *a)
{
    QString source;
    if (a == d->aTest1) {
        source = "<p>"
//                "<b><center>Simple token test</center></b></p><p>"
                "&nbsp;&nbsp;* Token D: ^$\"...~D~...\"$^<br />"
//                "&nbsp;&nbsp;* Null token: (^$All this text ~NULL~ should not appear in the output$^)<br />"
//                "&nbsp;&nbsp;* Token D without 'after conditionnal text':^$ ~D~$^<br />"
//                "&nbsp;&nbsp;* Token D without 'before conditionnal text': ^$~D~. $^<br />"
//                "&nbsp;&nbsp;* Long token A: ^$this text should appear in the output document, <u>including the core value</u> \"<b>~A~</b>\" (in bold) as defined in the <span style=' text-decoration: underline; color:#ff00ff;'>TokenModel</span>.$^<br />"
//                "&nbsp;&nbsp;* HTML Token:<br />"
//                "&nbsp;&nbsp;&nbsp;&nbsp;* Result should be \" <u><b>htmlToken</b></u> \"<br />"
//                "&nbsp;&nbsp;&nbsp;&nbsp;* Result should be ^$\" <u>~HTMLTOKEN~</u> \"$^<br />"
//                "^$ _D_ ~D~ _D_ $^<br />"
//                "<b>^$_<span style=' text-decoration: underline; color:#ff00ff;'>A_</span> ~A~ _A_$^ 10 chars </b><br />"
//                 "^$ <span style=' text-decoration: underline; color:#0000ff;'>_B_</span> ~B~ _B_$^ 10 chars <br />"
//                 " 10 chars ^$ _C_ ~C~ _C_$^<br />"
//                 " 10 chars ^$ _D_ ~D~ _D_$^<br />";
                ;
    } else if (a == d->aTest2) {
        source = "<p>"
                "<b><center>Nested tokens test</center></b></p><p>"
                "&nbsp;&nbsp;* Testing tokens:<br />"
                "&nbsp;&nbsp;&nbsp;&nbsp;* ^$\"Token B: (~B~) ^$[[Token ^$this text ~NULL~ should not appear in output$^C: ~C~]]$^.\"$^<br />"
                "&nbsp;&nbsp;* Result should be:<br />"
                "&nbsp;&nbsp;&nbsp;&nbsp;* \"Token B: (This is B) [[Token C: This is C]].\"<br />"
//                "^$ _^$‘‘nestedB’’~B~$^C_ ~C~ _C_$^<br />"
//                " 10 chars ^$ _D_ ~D~ _D_$^<br />"
                ;
    } else if (a == d->aTest3) {
        source = "<p><b>^$(<span style='text-decoration: underline; color:#ff00ff;'>A:</span> ~A~)$^. Some strings.</b><br />"
                "^$(<span style='text-decoration: underline; color:#0000ff;'>D:</span> ^$[C: ~C~]$^ ~D~)$^<br/>"
                "^$(B: ~B~)$^<br />";
    } else if (a == d->aTest4) {
    source = "<p><b>Testing tokens inside a table</b><br />"
            "<table border=1>"
            "<tr>"
            "  <td>^$_<span style=' text-decoration: underline; color:#ff00ff;'>A_</span> ~A~ _A_$^ 10 chars </td>"
            "</tr>"
            "<tr>"
            "  <td> 10 chars ^$ _D_ ~D~ _D_$^</td>"
            "</tr>"
            "</table>"
            "</p>";
    } else if (a == d->aTest5) {
    source = "<p><b>Testing nested tokens inside a table</b><br />"
            "<table border=1>"
            "<tr>"
            "  <td>^$<span style=' text-decoration: underline; color:#ff00ff;'>_A_</span> ~A~ _A_$^ 10 chars </td>"
            "</tr>"
            "<tr>"
            "  <td> 10 chars ^$ _D_ ~D~ _D_$^</td>"
            "</tr>"
            "<tr>"
            "  <td>Two nested: ^$ _D_ ~D~ _^$Nested C ~C~... $^D_$^</td>"
            "</tr>"
            "<tr>"
            "  <td>Multi-nested: ^$ _D_ ~D~ _^$Nested C ~C~..^$//~A~//$^.. $^D_$^</td>"
            "</tr>"
            "</table>"
            "</p>";
    }

//    if (d->_pad) {
//        delete d->_pad;
//        d->_pad = 0;
//    }
    d->ui->rawSource->setHtml(source);
    analyseRawSource();
}

QString PadWriter::htmlResult() const
{
    return d->ui->wysiwyg->toHtml();
}

QString PadWriter::rawSource() const
{
    return d->ui->rawSource->toPlainText();
}

void PadWriter::analyseRawSource()
{
    // clear PadDocument && views
    QList<Core::PadAnalyzerError> errors;
    d->_pad->clear();

    // Start analyze && token replacement
    PadAnalyzer().analyze(d->ui->rawSource->document(), d->_pad);
    d->_pad->setTokenModel(d->_tokenModel);
    d->_pad->run(d->_tokenModel->tokens());

    d->ui->listWidgetErrors->clear();
    foreach (const Core::PadAnalyzerError &error, errors) {
        switch (error.errorType()) {
        case Core::PadAnalyzerError::Error_UnexpectedChar:
            d->ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
            break;
        case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
            d->ui->listWidgetErrors->addItem(tr("Expected '%1' at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
            break;
        }
    }
}

void PadWriter::outputToRaw()
{
    // clear PadDocument && views
//    QList<Core::PadAnalyzerError> errors;
//    d->_pad->clear();

    // Start raw source composition
    d->_pad->toRaw();

//    PadAnalyzer().analyze(d->ui->rawSource->document(), d->_pad);
//    d->_pad->setTokenModel(d->_tokenModel);
//    d->_pad->run(d->_tokenModel->tokens());

//    d->ui->listWidgetErrors->clear();
//    foreach (const Core::PadAnalyzerError &error, errors) {
//        switch (error.errorType()) {
//        case Core::PadAnalyzerError::Error_UnexpectedChar:
//            d->ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
//            break;
//        case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
//            d->ui->listWidgetErrors->addItem(tr("Expected '%1' at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
//            break;
//        }
//    }
}

void PadWriter::viewErrors()
{
}

void PadWriter::highlightCursor()
{
//    int cursorPos;
//    // highlight cursor in output textdocument
//    cursorPos = d->ui->wysiwyg->textEdit()->textCursor().position();
//    PadFragment *outputFragment = d->_pad->padFragmentForOutputPosition(cursorPos);

//    // highlight cursor in source textdocument
//    cursorPos = d->ui->rawSource->textEdit()->textCursor().position();
//    PadFragment *rawFragment = d->_pad->padFragmentForSourcePosition(cursorPos);

//    QTextDocument *doc = d->ui->wysiwyg->document();
//    if (!item) {
//        if (d->_followedItem) {
//            Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//            d->_followedItem = 0;
//        }
//        return;
//    }

//    if (d->_followedItem) {
//        if (d->_followedItem == item)
//            return;
//        Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//        d->_followedItem = item;
//    } else {
//        d->_followedItem = item;
//    }
//    Constants::setPadFragmentFormat("Follow", d->_followedItem->outputStart(), d->_followedItem->outputEnd(), doc, d->_followedItemCharFormats, d->_followedCharFormat);

    // set timer
}

void PadWriter::findCursorPositionInOutput()
{
    if (!d->_pad)
        return;

//    // Find corresponding fragment Id
//    const int cursorPos = d->ui->rawSource->textEdit()->textCursor().position();
////    PadItem *item = d->_pad->padItemForSourcePosition(cursorPos);
//    PadFragment *item = d->_pad->padFragmentForSourcePosition(cursorPos);
//    QTextDocument *doc = d->ui->wysiwyg->document();
//    if (!item) {
//        if (d->_followedItem) {
//            Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//            d->_followedItem = 0;
//        }
//        return;
//    }

//    if (d->_followedItem) {
//        if (d->_followedItem == item)
//            return;
//        Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//        d->_followedItem = item;
//    } else {
//        d->_followedItem = item;
//    }
//    Constants::setPadFragmentFormat("Follow", d->_followedItem->outputStart(), d->_followedItem->outputEnd(), doc, d->_followedItemCharFormats, d->_followedCharFormat);
}

void PadWriter::setAutoUpdateOfResult(bool state)
{
    if (state) {
        connect(d->ui->rawSource->textEdit(), SIGNAL(textChanged()), this, SLOT(analyseRawSource()));
        analyseRawSource();
    } else {
        disconnect(d->ui->rawSource->textEdit(), SIGNAL(textChanged()), this, SLOT(analyseRawSource()));
    }
}

void PadWriter::setTestValues(bool /*state*/)
{
    analyseRawSource();
}

void PadWriter::onPadFragmentChanged(PadFragment *fragment)
{
    if (!d->_followedItem)
        return;
    if (d->_followedItem!=fragment)
        return;
//    // The followed fragment was modified
//    QTextDocument *doc = d->ui->wysiwyg->document();
//    Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//    Constants::setPadFragmentFormat("Follow", d->_followedItem->outputStart(), d->_followedItem->outputEnd(), doc, d->_followedItemCharFormats, d->_followedCharFormat);
}

void PadWriter::highLightNextBlock()
{
    WARN_FUNC;
    QTextCursor c(d->ui->wysiwyg->document());
    qWarning() << c.blockNumber();
    QTextBlockFormat f;
    f.setBackground(QBrush(QColor(Qt::white)));
    c.blockFormat().merge(f);

    f.setBackground(QBrush(QColor(Qt::lightGray)));
    QTextBlock b = c.block().next();
    b.blockFormat().merge(f);
}

void PadWriter::highLightPreviousBlock()
{
    WARN_FUNC;
    QTextCursor c(d->ui->wysiwyg->document());
    qWarning() << c.blockNumber();
    QTextBlockFormat f;
    f.setBackground(QBrush(QColor(Qt::white)));
    c.blockFormat().merge(f);

    f.setBackground(QBrush(QColor(Qt::lightGray)));
    QTextBlock b = c.block().previous();
    b.blockFormat().merge(f);
}
