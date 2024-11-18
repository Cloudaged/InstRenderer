
// You may need to build the project (run Qt uic code generator) to get "ui_MyToolBox.h" resolved

#include "MyToolBox.h"
#include "ui_MyToolBox.h"


MyToolBox::MyToolBox(QWidget *parent) :
        QWidget(parent), ui(new Ui::MyToolBox)
{
    ui->setupUi(this);

}

MyToolBox::~MyToolBox()
{
    delete ui;
}

void MyToolBox::Reconstruct(std::vector<QWidget*> contents)
{
    delete layout;
    layout = new QVBoxLayout(this);

    for (auto& content:contents)
    {
        MyToolPage* page = new MyToolPage(content,"MyName", this);

        layout->addWidget(page);
    }
    layout->addStretch();
    layout->setSpacing(0);
    this->setLayout(layout);
}

