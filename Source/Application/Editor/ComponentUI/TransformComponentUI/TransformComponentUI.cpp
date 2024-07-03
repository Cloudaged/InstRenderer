
// You may need to build the project (run Qt uic code generator) to get "ui_TransformComponentUI.h" resolved

#include "TransformComponentUI.h"
#include "ui_TransformComponentUI.h"


TransformComponentUI::TransformComponentUI(QWidget *parent) :
        QWidget(parent), ui(new Ui::TransformComponentUI)
{
    ui->setupUi(this);
}

TransformComponentUI::~TransformComponentUI()
{
    delete ui;
}
