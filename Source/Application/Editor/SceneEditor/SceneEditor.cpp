
// You may need to build the project (run Qt uic code generator) to get "ui_SceneEditor.h" resolved

#include "SceneEditor.h"
#include "ui_SceneEditor.h"


SceneEditor::SceneEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::SceneEditor)
{
    ui->setupUi(this);
    InitTreeWidget();
    ConnectFuncs();
    ItemChanged();
}



SceneEditor::~SceneEditor()
{
    delete ui;
}

void SceneEditor::InitTreeWidget()
{
    treeWidget = new SceneTree();
    treeWidget->setColumnCount(2);
    treeWidget->setHeaderLabels(QStringList()<<"Name"<<"Type"<<"ID");

    this->setWidget(treeWidget);

}

void SceneEditor::AddItem(int id,std::string name, std::string type)
{
    int idWithoutVersion = id & 0xFFFFF;

    QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
    item->setText(0,QString::fromStdString(name));
    item->setText(1,QString::fromStdString(type));
    item->setText(2,QString::fromStdString(std::to_string(id)));

    item->setFlags(item->flags() | Qt::ItemIsEditable);


    QIcon* icon = new QIcon("D:\\code_lib\\AGProject\\InstRenderer\\Source\\Application\\Editor\\icons\\GameObject.png");
    std::cout<<icon->name().toStdString();
    item->setIcon(0,*icon);

}

void SceneEditor::AddItem(int id, std::string name, std::string type, int parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0,QString::fromStdString(name));
    item->setText(1,QString::fromStdString(type));
    item->setText(2,QString::fromStdString(std::to_string(id)));

    item->setFlags(item->flags() | Qt::ItemIsEditable);

    QIcon* icon = new QIcon("D:\\code_lib\\AGProject\\InstRenderer\\Source\\Application\\Editor\\icons\\GameObject.png");
    std::cout<<icon->name().toStdString();
    item->setIcon(0,*icon);
    treeWidget->selectedItems()[0]->addChild(item);

}


void SceneEditor::DeleteItem(int id)
{
   if(treeWidget->curItem)
   {
       delete treeWidget->curItem;
       treeWidget->curItem = nullptr;
   }
}

QTreeWidgetItem* SceneEditor::FindItems(QTreeWidgetItem* topItem,int id)
{
    for(int i = 0;i <topItem->childCount();++i)
    {
        if(topItem->child(i)->text(2).toInt()==id)
        {
            delete topItem->child(i);
        }
        FindItems(topItem->child(i),id);
    }
    return nullptr;
}

void SceneEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu mainMenu(this);
    QMenu addMenu("Add Objects",this);

    mainMenu.addMenu(&addMenu);


    QAction *gameObjectAction = addMenu.addAction("GameObject");
    QAction *meshObjectAction = addMenu.addAction("MeshObject");
    QAction *lightAction = addMenu.addAction("Light");
    QAction *cameraAction = addMenu.addAction("Camera");

    if(!treeWidget->curItem)
    {
        connect(gameObjectAction, &QAction::triggered, this, [&]()
        {
            emit AddObjAction(GetUniqueName("GameObject"), "GameObject");
        });
        connect(meshObjectAction, &QAction::triggered, this, [&]()
        {
            emit AddObjAction("PewObj", "MeshObject");
        });
        connect(lightAction, &QAction::triggered, this, [&]()
        {
            emit AddObjAction("NewObj", "Light");
        });
        connect(cameraAction, &QAction::triggered, this, [&]()
        {
            emit AddObjAction("NewObj", "Camera");
        });
    }
    else
    {
        //Add objects
        connect(gameObjectAction, &QAction::triggered, this, [&]()
        {
            emit AddSubObjAction("Object","GameObject",treeWidget->curItem->text(2).toInt());
        });
        connect(meshObjectAction, &QAction::triggered, this, [&]()
        {
            emit AddSubObjAction("meshObject","GameObject",treeWidget->curItem->text(2).toInt());
        });
        connect(lightAction, &QAction::triggered, this, [&]()
        {
            emit AddSubObjAction("Object","GameObject",treeWidget->curItem->text(2).toInt());
        });
        connect(cameraAction, &QAction::triggered, this, [&]()
        {
            emit AddSubObjAction("Object","GameObject",treeWidget->curItem->text(2).toInt());
        });

        //Delete
        QAction *deleteAction = mainMenu.addAction("Delete");
        connect(deleteAction, &QAction::triggered, this, [&]()
        {
            emit DeleteObjAction(treeWidget->curItem->text(2).toInt());
        });

    }

    mainMenu.exec(event->globalPos());
}

void SceneEditor::CheckItem()
{
    connect(treeWidget, &QTreeWidget::itemClicked, this, [&](QTreeWidgetItem *item, int column)
    {
        treeWidget->curItem = item;
    });

}

void SceneEditor::ConnectFuncs()
{
    CheckItem();
}

std::string SceneEditor::GetUniqueName(std::string name)
{
    int count = 1;
    QString originalName = QString::fromStdString(name);
    QString uniqueName = originalName;
    while (!treeWidget->findItems(uniqueName, Qt::MatchExactly).isEmpty()) {
        uniqueName = QString("%1_%2").arg(originalName).arg(QString::number(count));
        ++count;
    }
    return uniqueName.toStdString();
}

void SceneEditor::ItemChanged()
{
    connect(treeWidget,&QTreeWidget::itemSelectionChanged,[&]()
    {
        if(!treeWidget->selectedItems().empty())
        {
            emit SelectedItemChanged(treeWidget->selectedItems()[0]->text(2).toInt());
        }

    });
}









