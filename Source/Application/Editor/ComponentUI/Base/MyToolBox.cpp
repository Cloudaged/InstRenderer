
// You may need to build the project (run Qt uic code generator) to get "ui_MyToolBox.h" resolved

#include "MyToolBox.h"
#include "ui_MyToolBox.h"


MyToolBox::MyToolBox(QWidget *parent) :
        QWidget(parent), ui(new Ui::MyToolBox)
{
    ui->setupUi(this);

    MyToolPage* page1 = new MyToolPage;
    MyToolPage* page2 = new MyToolPage;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(page1->pushButton);
    layout->addWidget(page1->contentWidget);
    layout->addWidget(page2->pushButton);
    layout->addWidget(page2->contentWidget);
    layout->addStretch();
    layout->setSpacing(0);


    this->setLayout(layout);

    //setLayout(layout);
}

MyToolBox::~MyToolBox()
{
    delete ui;
}
