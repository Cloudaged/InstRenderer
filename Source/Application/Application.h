#ifndef INSTRENDERER_APPLICATION_H
#define INSTRENDERER_APPLICATION_H
#include "QApplication"

#include "Editor/MainEditor/MainEditor.h"
#include "Presenter.h"

#include "Common/GameInstance.h"
#include "Common/GameType.h"
#include <string>
#include "QThread"
#include "GameThread.h"
#include "GlobalValue.h"

class Application
{
public:
    Application(std::string title,WindowSize size,int argc,char** argv);
    void Run();
    int Quit();
    std::string title;
    WindowSize windowSize;
private:
    bool isClose = false;
    void InitEditor();
    QApplication* qApplication;
    MainEditor* editor;
    GameInstance* gameInstance;
    Presenter* presenter;
    GameThread* gameThread;
};


#endif //INSTRENDERER_APPLICATION_H
