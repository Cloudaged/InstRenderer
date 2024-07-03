
// You may need to build the project (run Qt uic code generator) to get "ui_RenderEditor.h" resolved

#include "RenderEditor.h"
#include "ui_RenderEditor.h"


RenderEditor::RenderEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::RenderEditor)
{
    ui->setupUi(this);
}

RenderEditor::~RenderEditor()
{
    delete ui;
}
