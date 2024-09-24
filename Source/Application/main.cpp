#include "Application.h"
#include "QThread"
#include "Editor/ComponentUI/Custom/Transform3D.h"

int main(int argc,char** argv)
{
    Application app("InstRenderer",{1024,720},argc,argv);

    app.Run();

    return app.Quit();
}

