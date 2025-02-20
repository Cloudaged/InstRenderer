
// You may need to build the project (run Qt uic code generator) to get "ui_ContentWidget.h" resolved

#include "ContentWidget.h"
#include "ui_ContentWidget.h"

QListWidgetItem* ContentWidget::dragItem = nullptr;
ContentWidget::ContentWidget(QWidget *parent) :
        QListWidget(parent), ui(new Ui::ContentWidget)
{
    ui->setupUi(this);
}

ContentWidget::~ContentWidget()
{
    delete ui;
}


void ContentWidget::dropEvent(QDropEvent *event)
{

    QListWidget::dropEvent(event);
}

void ContentWidget::startDrag(Qt::DropActions supportedActions)
{
    dragItem=selectedItems()[0];
    QListView::startDrag(supportedActions);
}

void ContentWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
}
