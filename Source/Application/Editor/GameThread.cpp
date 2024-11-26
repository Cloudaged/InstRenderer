
// You may need to build the project (run Qt uic code generator) to get "ui_GameThread.h" resolved

#include "GameThread.h"


GameThread::GameThread(std::shared_ptr<GameInstance> instance,QThread *parent) :
        instance(instance),QThread(parent)
{

}

GameThread::~GameThread()
{
}

void GameThread::run()
{
    instance->Run();
    QThread::run();
}
