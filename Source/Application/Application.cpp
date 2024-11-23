
#include "Application.h"

Application::Application(std::string title, WindowSize size,int argc,char** argv): windowSize(size),editor(editor)
{
    //Locker
    Locker::Init();

    //Editor
    qApplication = new QApplication(argc,argv);
    gameInstance = new GameInstance({0,0});
    editor = new MainEditor(gameInstance->window);
    editor->resize(windowSize.width,windowSize.height);
    editor->isClose = &isClose;
    gameThread = new GameThread(gameInstance,&isClose);
    EditorInitializer initializer;
    editor->show();

    presenter = new Presenter(gameInstance,editor);
    //GameInstance


}

void Application::Run()
{
   gameThread->start();
}

int Application::Quit()
{
    qApplication->exec();
}

void Application::InitEditor()
{
    EditorInitializer initializer;
}




