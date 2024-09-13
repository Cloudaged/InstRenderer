
// You may need to build the project (run Qt uic code generator) to get "ui_ResourceEditor.h" resolved

#include "ResourceEditor.h"
#include "ui_ResourceEditor.h"


ResourceEditor::ResourceEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::ResourceEditor)
{
    ui->setupUi(this);
    InitListWidget();
}

ResourceEditor::~ResourceEditor()
{
    delete ui;
}

void ResourceEditor::InitListWidget()
{
    listWidget = new QListWidget();
    this->setWidget(listWidget);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    listWidget->setSpacing(20);
    listWidget->setIconSize(QSize(48,48));

    QIcon* icon = new QIcon("D:\\code_lib\\AGProject\\InstRenderer\\Source\\Editor\\icons\\GameObject.png");
    QListWidgetItem* item = new QListWidgetItem(listWidget);
    item->setIcon(*icon);
    item->setText("ResMesh");
}
