
#ifndef INSTRENDERER_GAMETHREAD_H
#define INSTRENDERER_GAMETHREAD_H

#include <QThread>
#include "iostream"
#include "Application.h"

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
    explicit GameThread(Application* application,QThread *parent = nullptr);

    ~GameThread() override;

    void run() override;

private:
    Application* application;
};


#endif //INSTRENDERER_GAMETHREAD_H
