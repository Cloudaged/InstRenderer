
// You may need to build the project (run Qt uic code generator) to get "ui_Transform3D.h" resolved

#include "Transform3D.h"
#include "ui_Transform3D.h"


Transform3D::Transform3D(std::string tagName,glm::vec3 data,QWidget *parent) :
        QWidget(parent), ui(new Ui::Transform3D),tagName(tagName), data(data)
{
    ui->setupUi(this);

    InitWidget();
}

Transform3D::~Transform3D()
{
    delete ui;
}

void Transform3D::InitWidget()
{
    setFocusPolicy(Qt::ClickFocus);

    tag.setText(tagName.c_str());

    xLabel = new QLabel("x");
    yLabel = new QLabel("y");
    zLabel = new QLabel("z");

    xEdit = new QLineEdit(QString::number(data.x), this);
    yEdit = new QLineEdit(QString::number(data.y), this);
    zEdit = new QLineEdit(QString::number(data.z), this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&tag);
    layout->addSpacing(10);
    layout->addWidget(xLabel);
    layout->addWidget(xEdit);
    layout->addWidget(yLabel);
    layout->addWidget(yEdit);
    layout->addWidget(zLabel);
    layout->addWidget(zEdit);

    this->setLayout(layout);
}

void Transform3D::UpdateData(glm::vec3 data)
{
    this->data = data;
    xEdit->setText(QString::number(data.x));
    yEdit->setText(QString::number(data.y));
    zEdit->setText(QString::number(data.z));
}


