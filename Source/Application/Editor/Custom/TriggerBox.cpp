
// You may need to build the project (run Qt uic code generator) to get "ui_TriggerBox.h" resolved

#include "TriggerBox.h"
#include "ui_TriggerBox.h"


TriggerBox::TriggerBox(std::string name,bool data,QWidget *parent) :
        QWidget(parent), ui(new Ui::TriggerBox),value(data)
{
    ui->setupUi(this);
    this->tag = new QLabel(QString::fromStdString(name), this);
    this->trigger = new QCheckBox(this);
    UpdateData(data);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(tag);
    layout->addSpacing(200);
    layout->addWidget(trigger);
    this->setLayout(layout);
}

TriggerBox::~TriggerBox()
{
    delete ui;
}

void TriggerBox::UpdateData(bool value)
{
    if(value)
    {
        this->trigger->setCheckState(Qt::Checked);
    }else
    {
        this->trigger->setCheckState(Qt::Unchecked);
    }
}
