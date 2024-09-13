#include "Application.h"
#include "QThread"
#include "Editor/ComponentUI/Custom/Transform3D.h"
#include "GameThread.h"
void Loop()
{
    while(1)
    {
        std::cout << "Hello\n";
        QThread::msleep(10);
    }
    //std::cout << "Hello\n";
}
int main(int argc,char** argv)
{
    QApplication qA(argc, argv);
    MainEditor* editor = new MainEditor();
    editor->show();
    Application app(editor,"InstRenderer",{1024,720},argc,argv);

    GameThread gameThread(&app);
    gameThread.start();


    return QApplication::exec();
}

