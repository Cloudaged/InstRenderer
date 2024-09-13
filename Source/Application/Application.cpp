
#include "Application.h"

Application::Application(MainEditor* editor,std::string title, WindowSize size,int argc,char** argv): windowSize(size),editor(editor)
{

    //Editor
    qApplication = new QApplication(argc,argv);
    gameInstance = new GameInstance(windowSize);

    EditorInitializer initializer;


    presenter = new Presenter(gameInstance,editor);
    //GameInstance


}

void Application::Run()
{
    while (1)
    {
        gameInstance->Tick();
    }
}

int Application::Quit()
{
    return qApplication->exec();
}

void Application::InitEditor()
{
    EditorInitializer initializer;
}




