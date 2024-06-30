
// You may need to build the project (run Qt uic code generator) to get "ui_SceneEditor.h" resolved

#include "SceneEditor.h"
#include "ui_SceneEditor.h"


SceneEditor::SceneEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::SceneEditor)
{
    ui->setupUi(this);
    InitTreeWidget();
}



SceneEditor::~SceneEditor()
{
    delete ui;
}

void SceneEditor::InitTreeWidget()
{
    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(2);
    treeWidget->setHeaderLabels(QStringList()<<"Name"<<"Type");

    this->setWidget(treeWidget);

    AddItem("MyItem","GameObject");
}

void SceneEditor::AddItem(std::string name, std::string type)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
    item->setText(0,QString::fromStdString(name));
    item->setText(1,QString::fromStdString(type));
    QIcon* icon = new QIcon("D:\\code_lib\\AGProject\\InstRenderer\\Source\\Editor\\icons\\GameObject.png");
    std::cout<<icon->name().toStdString();
    item->setIcon(0,*icon);

}



void SceneEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction* addItemAction = menu.addAction("Add Item");

    connect(addItemAction,&QAction::triggered, this,&SceneEditor::AddObjAction);
    menu.exec(event->globalPos());
    //QWidget::contextMenuEvent(event);
}
