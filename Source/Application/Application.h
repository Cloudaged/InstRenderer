#ifndef INSTRENDERER_APPLICATION_H
#define INSTRENDERER_APPLICATION_H
#include "QApplication"
#include "Editor/MainEditor/MainEditor.h"
#include "Presenter.h"
#include "Common/GameInstance.h"
#include <string>
#include "QThread"
#include "Editor/GameThread.h"

class Application
{
public:
    Application(std::string title,WindowSize size,int argc,char** argv);
    ~Application();
    void Run();
    int Quit();
public:
    std::string title;
    std::shared_ptr<WindowContext> windowContext;
private:
    std::shared_ptr<GameInstance> gameInstance;

#ifdef WITH_EDITOR
    std::unique_ptr<QApplication> qApplication;
    std::shared_ptr<MainEditor> editor;
    std::shared_ptr<Presenter> presenter;
#endif

private:
    void InitWindow();
};


#endif //INSTRENDERER_APPLICATION_H
