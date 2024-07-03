
// You may need to build the project (run Qt uic code generator) to get "ui_SceneTree.h" resolved

#include "SceneTree.h"
#include "ui_SceneTree.h"


SceneTree::SceneTree(QTreeWidget *parent) :
        QTreeWidget(parent), ui(new Ui::SceneTree)
{
    ui->setupUi(this);

    setEditTriggers(QTreeWidget::NoEditTriggers);
}

SceneTree::~SceneTree()
{
    delete ui;
}

void SceneTree::mousePressEvent(QMouseEvent *event)
{
    QTreeWidgetItem *item = itemAt(event->pos());
    if ((!item)&&!selectedItems().empty()) {
        curItem = nullptr;
        selectedItems()[0]->setSelected(false);
    }

    QTreeView::mousePressEvent(event);
}

void SceneTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if(item)
    {
       editItem(item,0);
    }
}

void SceneTree::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(selectedItems().empty())
    {
        emit ItemRenamed(selectedItems()[0]->text(2).toInt(),selectedItems()[0]->text(0).toStdString());
    }
    QAbstractItemView::closeEditor(editor, hint);
}
