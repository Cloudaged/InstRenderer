
#include "GameInstance.h"
#include "iostream"

#include "../Resource/ResourceManager.h"


GameInstance::GameInstance(WindowSize size): size(size)
{
    InitCore();
    InitWindow(size);
    controller = new Controller(&event);
    InitVulkanContext();
    InitEntity();
    mainScene->InitGlobalSet();
    mainScene->InitSceneData();
    mainScene->InitSkyboxData();
    mainScene->InitMainLight();
    InitSystem();
}

void GameInstance::InitWindow(WindowSize size)
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        std::cout<<"Failed to create sdl window\n";
    }

    window = SDL_CreateWindow("GameInstance",
                              SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                              size.width,size.height,
                              SDL_WINDOW_SHOWN|SDL_WINDOW_VULKAN|SDL_WINDOW_RESIZABLE);

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);


}

void GameInstance::InitCore()
{
    //Meta
    RegisterMeta();
    //Scene
    mainScene = new Scene;
    //Registry
    entityManager = new EntityManager(&mainScene->reg);
    //Locker

    //Resource Manager
    ResourceManager::Init();


}

void GameInstance::InitVulkanContext()
{
    VulkanContext::Init(window,&event);
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

void GameInstance::Run(bool* isClose)
{
    while ((!(*isClose)))
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

                size.width= event.window.data1;
                size.height = event.window.data2;
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



