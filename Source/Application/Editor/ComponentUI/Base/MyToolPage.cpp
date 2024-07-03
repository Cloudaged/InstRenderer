
// You may need to build the project (run Qt uic code generator) to get "ui_MyToolPage.h" resolved

#include "MyToolPage.h"
#include "ui_MyToolPage.h"


MyToolPage::MyToolPage(QWidget *parent) :
        QWidget(parent), ui(new Ui::MyToolPage)
{
    ui->setupUi(this);

    QPushButton* pushButton1 = new QPushButton();
    QPushButton* pushButton2 = new QPushButton;
    QPushButton* pushButton3 = new QPushButton;


    contentWidget = new QWidget(this);

    pushButton = new QPushButton(this);
    pushButton->setText("Hello Trigger");
    layout = new QVBoxLayout;
    layout->addWidget(pushButton1);
    layout->addWidget(pushButton2);
    layout->addWidget(pushButton3);


    contentWidget->setLayout(layout);

    connect(pushButton, &QPushButton::clicked,
            this, &MyToolPage::OnPushButtonFoldClicked);

}

MyToolPage::~MyToolPage()
{
    delete ui;
}

void MyToolPage::AddWidget(const QString &title, QWidget *widget)
{

}

void MyToolPage::Expand()
{
    std::cout<<"expaned";
    contentWidget->show();
    isExpanded = true;

}

void MyToolPage::Fold()
{
    contentWidget->hide();
    //this->resize(pushButton->width(),pushButton->height());
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
