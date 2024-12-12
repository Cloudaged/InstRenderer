
// You may need to build the project (run Qt uic code generator) to get "ui_DragableLabel.h" resolved

#include "DragableLabel.h"
#include "ui_DragableLabel.h"


DragableLabel::DragableLabel(QString text,QWidget *parent) :
        QLabel(text,parent), ui(new Ui::DragableLabel)
{
    ui->setupUi(this);

}

DragableLabel::~DragableLabel()
{
    delete ui;
}

void DragableLabel::enterEvent(QEnterEvent *event)
{
    setCursor(Qt::SizeHorCursor);
    QLabel::enterEvent(event);
}

void DragableLabel::leaveEvent(QEvent *event)
{
    unsetCursor();
    QLabel::leaveEvent(event);
}

void DragableLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDrag = false;
        lastMousePosition = event->pos();
    }
    QLabel::mousePressEvent(event);
}

void DragableLabel::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->pos().x() - lastMousePosition.x();
    value = dx;
    lastMousePosition = event->pos();
    emit ChangeData(value);
    QLabel::mouseMoveEvent(event);
}

void DragableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        value=0;
        isDrag = false;
    }
    QLabel::mouseReleaseEvent(event);
}
