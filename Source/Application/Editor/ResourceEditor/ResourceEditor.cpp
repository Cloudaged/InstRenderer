
// You may need to build the project (run Qt uic code generator) to get "ui_ResourceEditor.h" resolved

#include "ResourceEditor.h"
#include "ui_ResourceEditor.h"


ResourceEditor::ResourceEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::ResourceEditor)
{
    ui->setupUi(this);
    InitFolderWidget();
    InitListWidget();
}

ResourceEditor::~ResourceEditor()
{
    delete ui;
}

void ResourceEditor::InitListWidget()
{

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    contentWidget = new ContentWidget();
    contentWidget->setViewMode(QListView::IconMode);
    contentWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    contentWidget->setSpacing(10);
    contentWidget->setIconSize(QSize(48, 48));
    contentWidget->setGridSize(QSize(100, 100));
    contentWidget->setDragEnabled(true);
    contentWidget->setDropIndicatorShown(true);


    splitter->addWidget(folderWidget);
    splitter->addWidget(contentWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    this->setWidget(splitter);

    connect(contentWidget, &QListWidget::itemDoubleClicked, this, [&]()
    {
        auto item = contentWidget->selectedItems()[0];
        selectedPath=item->data(Qt::UserRole).toString().toStdString();
        ShowFileOfPath(selectedPath);
    });

}

void ResourceEditor::InitFolderWidget()
{
    folderWidget = new QTreeWidget();

    QDir dir(QString::fromStdString(FILE_PATH("Asset")));


    folderWidget->setHeaderLabel("Hierarchy");
    std::function<void(QTreeWidgetItem*, const QString&)> addItems = [&](QTreeWidgetItem* parentItem, const QString& path) {
        QDir dir(path);
        QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        for (const QFileInfo& fileInfo : fileList) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, fileInfo.fileName());
            item->setData(0, Qt::UserRole, fileInfo.filePath());
            item->setIcon(0,*IconManager::GetIcon("Folder.png"));
            // 如果是目录，递归添加子项
            if (fileInfo.isDir()) {
                parentItem->addChild(item);
                addItems(item, fileInfo.filePath());
            }
        }
    };

    // 添加根目录
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(folderWidget);
    rootItem->setText(0, dir.dirName());
    rootItem->setData(0, Qt::UserRole, dir.path());
    rootItem->setIcon(0,*IconManager::GetIcon("Folder.png"));

    folderWidget->addTopLevelItem(rootItem);

    addItems(rootItem,QString::fromStdString(FILE_PATH("Asset")));
    folderWidget->expandAll();


    connect(folderWidget,&QTreeWidget::itemClicked,[&]()
    {
        selectedPath=folderWidget->selectedItems()[0]->data(0, Qt::UserRole).toString().toStdString();
        ShowFileOfPath(selectedPath);
    });
}

void ResourceEditor::ShowFileOfPath(std::string path)
{
    QDir dir(QString::fromStdString(path));

    if(!dir.exists()) return;

    QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files| QDir::Dirs);
    
    contentWidget->clear();

    for (const QFileInfo& fileInfo : fileList)
    {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName());

        if(fileInfo.isDir())
        {
            item->setData(Qt::UserRole, fileInfo.filePath());
            item->setIcon(*IconManager::GetIcon("Folder.png"));
            contentWidget->addItem(item);
        }
    }
    for (const QFileInfo& fileInfo : fileList)
    {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName());

        if(!fileInfo.isDir())
        {
            item->setData(Qt::UserRole, fileInfo.filePath());
            /*item->setIcon(*IconManager::GetIcon("File.png"));*/
            SetIcon(item,fileInfo.suffix());
            contentWidget->addItem(item);
        }
    }
}

void ResourceEditor::SetIcon(QListWidgetItem* item, QString extension)
{

    if (extension=="fbx")
    {
        item->setIcon(*IconManager::GetIcon("Model.png"));
    }
    else if(extension=="png")
    {
        item->setIcon(*IconManager::GetIcon("Texture.png"));
    }
    else
    {
        item->setIcon(*IconManager::GetIcon("File.png"));
    }
}


