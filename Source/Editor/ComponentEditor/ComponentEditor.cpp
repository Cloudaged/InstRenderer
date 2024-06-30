
// You may need to build the project (run Qt uic code generator) to get "ui_ComponentEditor.h" resolved

#include "ComponentEditor.h"
#include "ui_ComponentEditor.h"


ComponentEditor::ComponentEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::ComponentEditor)
{
    ui->setupUi(this);
    InitToolBox();
}

ComponentEditor::~ComponentEditor()
{
    delete ui;
}

void ComponentEditor::InitToolBox()
{
    scrollArea = new QScrollArea();

    QWidget* res = new QWidget();

    toolBox = new MyToolBox();

    scrollArea->setWidget(toolBox);


    //scrollArea->setWidget(toolBox);
    this->setWidget(scrollArea);
}
