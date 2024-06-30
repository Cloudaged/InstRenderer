
// You may need to build the project (run Qt uic code generator) to get "ui_ConsoleEditor.h" resolved

#include "ConsoleEditor.h"
#include "ui_ConsoleEditor.h"


ConsoleEditor::ConsoleEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::ConsoleEditor)
{
    ui->setupUi(this);
    InitTextWidget();
}

ConsoleEditor::~ConsoleEditor()
{
    delete ui;
}

void ConsoleEditor::InitTextWidget()
{
    textWidget = new QPlainTextEdit(this);
    textWidget->setReadOnly(true);
    this->setWidget(textWidget);
    AddMessage();
}

void ConsoleEditor::AddMessage()
{
    QTextCharFormat fmt;

    fmt.setForeground(QBrush(Qt::red));

    textWidget->mergeCurrentCharFormat(fmt);
    textWidget->appendPlainText("HelloWorld");
    textWidget->appendPlainText("Next Hello World");
}
