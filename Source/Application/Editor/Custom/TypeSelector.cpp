
// You may need to build the project (run Qt uic code generator) to get "ui_TypeSelector.h" resolved

#include "TypeSelector.h"
#include "ui_TypeSelector.h"


TypeSelector::TypeSelector(std::string name,QWidget *parent) :
        QWidget(parent), ui(new Ui::TypeSelector)
{
    ui->setupUi(this);
    this->tag = new QLabel(QString::fromStdString(name));
    this->typer = new QComboBox();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(tag);
    layout->addSpacing(10);
    layout->addWidget(typer);
    this->setLayout(layout);

}

TypeSelector::~TypeSelector()
{
    delete ui;
}

void TypeSelector::AddNewType(std::string name)
{
    this->typer->addItem(QString::fromStdString(name));
}

void TypeSelector::UpdateData(std::string data)
{
    auto index = this->typer->findText(QString::fromStdString(data));
    this->typer->setCurrentIndex(index);
}
