
// You may need to build the project (run Qt uic code generator) to get "ui_ValueEdit.h" resolved

#include "ValueEdit.h"
#include "ui_ValueEdit.h"


ValueEdit::ValueEdit(std::string name,float data,QWidget *parent) :
        QWidget(parent), ui(new Ui::ValueEdit),data(data)
{
    ui->setupUi(this);

    tag = new DragableLabel(QString::fromStdString(name));
    edit = new QLineEdit();

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(tag);
    layout->setSpacing(100);
    layout->addWidget(edit);
    this->setLayout(layout);

    connect(this->tag,&DragableLabel::ChangeData,[&](float value)
    {
        auto temp = edit->text().toFloat();
        temp+= value*1;
        edit->setText(QString::number(temp));
        emit DragEditFinished(edit->text().toFloat());
    });
}

ValueEdit::~ValueEdit()
{
    delete ui;
}

void ValueEdit::UpdateData(float data)
{
    this->data = data;
    edit->setText(QString::number(data));
}
