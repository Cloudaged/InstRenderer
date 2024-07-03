
#include "GameInstance.h"
#include "iostream"
GameInstance::GameInstance(WindowSize size)
{
    RegisterMeta();
    mainScene = new Scene;
}



