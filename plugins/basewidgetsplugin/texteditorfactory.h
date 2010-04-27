#ifndef TEXTEDITORFACTORY_H
#define TEXTEDITORFACTORY_H

#include <texteditorplugin/texteditor.h>

#include <formmanagerplugin/iformwidgetfactory.h>

#include <QStringList>

namespace BaseWidgets {

class TextEditorFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    TextEditorFactory(QObject *parent = 0);
    ~TextEditorFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class TextEditorForm : public Form::IFormWidget
{
    Q_OBJECT
public:
    TextEditorForm(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~TextEditorForm();

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return true;}

    void setValue(const QVariant &) {}
    QVariant value() const;

public Q_SLOTS:
    void retranslate();

private:
    Editor::TextEditor *m_Text;
};

}  // End namespace BaseWidgets

#endif // TEXTEDITORFACTORY_H
