
#include "Application.h"

Application::Application(std::string title, WindowSize size,int argc,char** argv): windowSize(size)
{

    //Editor
    qApplication = new QApplication(argc,argv);
    editor = new MainEditor();
    gameInstance = new GameInstance(windowSize);
    //GameInstance


}

void Application::Run()
{
    Opts();

}

int Application::Quit()
{
    return qApplication->exec();
}

void Application::Opts()
{
    AddNode();
}

void Application::AddNode()
{
    editor->connect(editor,&MainEditor::AddNewNode,[&]()
    {
        std::cout<<"Add nodes\n";
    });
}


