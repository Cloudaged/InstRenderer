
#ifndef INSTRENDERER_GAMETHREAD_H
#define INSTRENDERER_GAMETHREAD_H

#include <QThread>
#include "iostream"
#include "Common/GameInstance.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class GameThread;
}
QT_END_NAMESPACE


class GameThread : public QThread
{
Q_OBJECT

public:
    GameThread(std::shared_ptr<GameInstance> instance,QThread *parent = nullptr);
    ~GameThread() override;
    void run() override;
private:
    std::shared_ptr<GameInstance> instance;
};


#endif //INSTRENDERER_GAMETHREAD_H
