#ifndef INSTRENDERER_APPLICATION_H
#define INSTRENDERER_APPLICATION_H
#include "QApplication"

#include "MainEditor/MainEditor.h"
#include "Common/GameInstance.h"
#include "Common/GameType.h"
#include <string>


class  Application
{
public:
    Application(std::string title,WindowSize size,int argc,char** argv);
    void Run();
    int Quit();
    std::string title;
    WindowSize windowSize;
private:

    void Opts();
    void AddNode();

    QApplication* qApplication;
    MainEditor* editor;
    GameInstance* gameInstance;

    void AddObjects();
};


#endif //INSTRENDERER_APPLICATION_H
