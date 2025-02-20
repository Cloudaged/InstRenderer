
// You may need to build the project (run Qt uic code generator) to get "ui_MainEditor.h" resolved

#include "MainEditor.h"
#include "ui_MainEditor.h"


MainEditor::MainEditor(std::shared_ptr<WindowContext> windowContext,QWidget *parent) :
        QMainWindow(parent),ui(new Ui::MainEditor),windowContext(windowContext)
{
    ui->setupUi(this);
    DeleteCentralWidget();
    InitCustomTitleBar();
    InitSubWidget();
    InitSubWidgetLayoutAndShow();
    graphicSettingWidget = new GraphicSettingWidget();
   // tabifyDockWidget(componentEditor,graphicSettingWidget);

    resize(windowContext->windowSize.width,windowContext->windowSize.height);
    this->show();

}

void MainEditor::InitCustomTitleBar()
{
    this->setWindowFlag(Qt::FramelessWindowHint);

    layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    titleBar = new CustomTitleBar(this);
    titleBar->mainEditor = this;
    setMenuWidget(titleBar);
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
    renderEditor = new RenderEditor(windowContext->window);

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


void MainEditor::closeEvent(QCloseEvent *event)
{
    windowContext->isClose = true;
    gameThread->quit();
    event->accept();
}

void MainEditor::StartGameThread(std::shared_ptr<GameInstance> gameInstance)
{
    gameThread = std::make_unique<GameThread>(gameInstance);
    gameThread->start();
}










