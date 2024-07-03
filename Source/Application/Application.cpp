
#include "Application.h"

Application::Application(std::string title, WindowSize size,int argc,char** argv): windowSize(size)
{

    //Editor
    qApplication = new QApplication(argc,argv);
    gameInstance = new GameInstance(windowSize);
    editor = new MainEditor();

    presenter = new Presenter(gameInstance,editor);
    //GameInstance


}

void Application::Run()
{

}

int Application::Quit()
{
    return qApplication->exec();
}




