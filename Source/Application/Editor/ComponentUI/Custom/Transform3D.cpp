
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

    xLabel = new DragableLabel("x");
    yLabel = new DragableLabel("y");
    zLabel = new DragableLabel("z");

    QRegularExpression regExp("^-?[0-9]+$");
    QRegularExpressionValidator *regExpValidator = new QRegularExpressionValidator(regExp, this);

    xEdit = new QLineEdit(QString::number(data.x), this);
    yEdit = new QLineEdit(QString::number(data.y), this);
    zEdit = new QLineEdit(QString::number(data.z), this);

    xEdit->setValidator(regExpValidator);
    yEdit->setValidator(regExpValidator);
    zEdit->setValidator(regExpValidator);

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

    connect(this->xLabel,&DragableLabel::ChangeData,[&](float value)
    {
        auto temp = xEdit->text().toFloat();
        temp+= value*xWeight;
        xEdit->setText(QString::number(temp));
        emit DragEditFinished({xEdit->text().toFloat(), yEdit->text().toFloat(), zEdit->text().toFloat()});
    });
    connect(this->yLabel,&DragableLabel::ChangeData,[&](float value)
    {
        auto temp = yEdit->text().toFloat();
        temp+= value*yWeight;
        yEdit->setText(QString::number(temp));
        emit DragEditFinished({xEdit->text().toFloat(), yEdit->text().toFloat(), zEdit->text().toFloat()});

    });
    connect(this->zLabel,&DragableLabel::ChangeData,[&](float value)
    {
        auto temp = zEdit->text().toFloat();
        temp+= value*zWeight;
        zEdit->setText(QString::number(temp));
        emit DragEditFinished({xEdit->text().toFloat(), yEdit->text().toFloat(), zEdit->text().toFloat()});

    });
}

void Transform3D::UpdateData(glm::vec3 data)
{
    this->data = data;
    xEdit->setText(QString::number(data.x));
    yEdit->setText(QString::number(data.y));
    zEdit->setText(QString::number(data.z));
}


