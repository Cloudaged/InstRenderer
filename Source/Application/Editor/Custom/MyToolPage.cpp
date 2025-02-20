
// You may need to build the project (run Qt uic code generator) to get "ui_MyToolPage.h" resolved

#include "MyToolPage.h"
#include "ui_MyToolPage.h"


MyToolPage::MyToolPage(QWidget* content,std::string name,QWidget *parent) :
        QWidget(parent), ui(new Ui::MyToolPage)
{
    ui->setupUi(this);

    this->contentWidget = content;
    pushButton = new QPushButton(name.c_str(),this);

    layout = new QVBoxLayout(this);

    contentParent = content->parent();

    layout->addWidget(pushButton);
    layout->addWidget(contentWidget);


    this->setLayout(layout);

    connect(pushButton, &QPushButton::clicked,
            this, &MyToolPage::OnPushButtonFoldClicked);

}

MyToolPage::~MyToolPage()
{
    layout->removeWidget(contentWidget);
    contentWidget->setParent((QWidget*)contentParent);
    delete ui;
}

void MyToolPage::AddWidget(const QString &title, QWidget *widget)
{

}

void MyToolPage::Expand()
{
    contentWidget->show();
    isExpanded = true;

}

void MyToolPage::Fold()
{
    contentWidget->hide();
    isExpanded = false;
}

void MyToolPage::OnPushButtonFoldClicked()
{
    if(isExpanded)
    {
        Fold();
    }
    else
    {
        Expand();
    }
}


