
// You may need to build the project (run Qt uic code generator) to get "ui_GameThread.h" resolved

#include "GameThread.h"

GameThread::GameThread(Application* application,QThread *parent) :
        application(application),QThread(parent)
{

}

GameThread::~GameThread()
{
}

void GameThread::run()
{
   application->Run();
    QThread::run();
}
