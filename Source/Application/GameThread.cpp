
// You may need to build the project (run Qt uic code generator) to get "ui_GameThread.h" resolved

#include "GameThread.h"

GameThread::GameThread(GameInstance* instance,bool* closeTrigger,QThread *parent) :
        instance(instance),isClose(closeTrigger),QThread(parent)
{

}

GameThread::~GameThread()
{
}

void GameThread::run()
{
    instance->Run(isClose);
    QThread::run();
}
