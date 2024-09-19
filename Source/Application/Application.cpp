
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
    while (!editor->isClose)
    {
        gameInstance->Tick();
    }
    std::cout<<"QUIT\n";
}

int Application::Quit()
{
    return qApplication->exec();
}

void Application::InitEditor()
{
    EditorInitializer initializer;
}




