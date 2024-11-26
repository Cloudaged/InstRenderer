
#include "GameInstance.h"
#include "iostream"

#include "../Resource/ResourceManager.h"


GameInstance::GameInstance(std::shared_ptr<WindowContext> windowContext): windowContext(windowContext)
{
    InitCore();
    controller = new Controller(&event);
    InitVulkanContext();
    InitEntity();
    aaaabbbbb
    mainScene->InitGlobalSet();
    mainScene.InitSceneData();
    mainScene.InitSkyboxData();
    mainScene.InitMainLight();
    InitSystem();
}

void GameInstance::InitCore()
{
    Locker::Init();
    //Meta
    RegisterMeta();
    //Resource Manager
    ResourceManager::Init();

}

void GameInstance::InitVulkanContext()
{
    VulkanContext::Init(windowContext->window,&event);
}

void GameInstance::InitSystem()
{
    //Render System
    RenderSysBeginInfo renderSysInfo{};
    renderSysInfo.registry = &mainScene->reg;
    renderSysInfo.globalData = mainScene->globalData;
    renderSystem.BeginSystem(renderSysInfo);

    resourceSystem.BeginSystem();
}

void GameInstance::Tick()
{
    SDLEvent();
    if(!isRun)
        return;
    if(!isReady)
    {
        isReady = true;
        return;
    }
    mainScene->UpdateScene();
    renderSystem.Execute();
}

void GameInstance::InitEntity()
{
}

void GameInstance::Run()
{
    while (!windowContext->isClose)
    {
        Tick();
    }
}

void GameInstance::SDLEvent()
{
    while (SDL_PollEvent(&event))
    {
        controller->ViewInteract(&mainScene->mainCamera);

        if(event.type==SDL_WINDOWEVENT)
        {
            if(event.window.event==SDL_WINDOWEVENT_RESIZED)
            {
                std::lock_guard<std::mutex> lock(Locker::Get().resizeMtx);

                windowContext->windowSize.width= event.window.data1;
                windowContext->windowSize.height = event.window.data2;
                mainScene->UpdateAspect();
            }
            else if(event.window.event ==SDL_WINDOWEVENT_MINIMIZED)
            {
                isRun= false;
            }
            else if(event.window.event==SDL_WINDOWEVENT_RESTORED)
            {
                isRun= true;
            }
        }
    }
}



