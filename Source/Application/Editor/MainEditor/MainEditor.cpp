
// You may need to build the project (run Qt uic code generator) to get "ui_MainEditor.h" resolved

#include "MainEditor.h"
#include "ui_MainEditor.h"


MainEditor::MainEditor(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainEditor)
{
    ui->setupUi(this);
    DeleteCentralWidget();
    InitMenuBar();
    InitSubWidget();
    InitSubWidgetLayoutAndShow();


}

MainEditor::~MainEditor()
{
    delete ui;
}

void MainEditor::InitSubWidget()
{
    setDockNestingEnabled(true);

    //Scene Editor
    sceneEditor = new SceneEditor();
    //Component Editor
    componentEditor = new ComponentEditor();

    //Render Editor
    renderEditor = new RenderEditor();

    //Resource Editor
    resourceEditor = new ResourceEditor();

    //Console Editor
    consoleEditor = new ConsoleEditor();

}

void MainEditor::DeleteCentralWidget()
{
    QWidget* p = takeCentralWidget();
    if(p)
        delete p;
}

void MainEditor::InitSubWidgetLayoutAndShow()
{
    //Split
    this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea,sceneEditor,Qt::Orientation::Horizontal);
    splitDockWidget(sceneEditor,componentEditor,Qt::Horizontal);
    splitDockWidget(sceneEditor,resourceEditor,Qt::Vertical);
    splitDockWidget(sceneEditor,renderEditor,Qt::Horizontal);
    tabifyDockWidget(resourceEditor,consoleEditor);
    resourceEditor->raise();

    //Show
    this->show();

    //Resize
    //Width
    QList<QDockWidget*> docks ={sceneEditor,renderEditor,componentEditor};
    QList<int> size = {(int)(this->width()*0.2),(int)(this->width()*0.6),(int)(this->width()*0.2)};
    this->resizeDocks(docks,size,Qt::Horizontal);

    //Height
    docks = {sceneEditor,renderEditor,resourceEditor};
    size = {(int)(this->height()*0.8),(int)(this->height()*0.8),(int)(this->height()*0.2)};
    this->resizeDocks(docks,size,Qt::Vertical);


}

void MainEditor::InitMenuBar()
{
    menuBar = new QMenuBar(this);
    fileMenu = new QMenu("file",menuBar);
    menuBar->addMenu(fileMenu);

    this->setMenuBar(menuBar);
}





