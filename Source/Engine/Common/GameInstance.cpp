
#include "GameInstance.h"
#include "iostream"
#include "../Resource/ResourceManager.h"


GameInstance::GameInstance(std::shared_ptr<WindowContext> windowContext): windowContext(windowContext)
{
    InitCore();
    InitVulkanContext();

    mainScene->InitGlobalSet();
    mainScene->InitSceneData();
    mainScene->InitSkyboxData();
    mainScene->InitMainLight();
    InitSystem();
}



void GameInstance::InitCore()
{
    Locker::Init();
    //Meta
    controller = new Controller(&event);
    RegisterMeta();
    //Scene
    mainScene = std::make_shared<Scene>();
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
    ReceiveEvent();
    Update();
    if(AllowToTick())
    {
        Execute();
    }
}

void GameInstance::Run()
{
    while (!windowContext->isClose)
    {
        Tick();
    }
}

void GameInstance::ReceiveEvent()
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
                windowContext->isMinimal= false;
            }
            else if(event.window.event==SDL_WINDOWEVENT_RESTORED)
            {
                windowContext->isMinimal= true;
            }
        }
    }
}

bool GameInstance::AllowToTick()
{
    static int isFirstFrame = true;
    if(isFirstFrame)
    {
        isFirstFrame = false;
    }
    return (!isFirstFrame)&&windowContext->isMinimal;
}

void GameInstance::Update()
{
    mainScene->UpdateScene();

}

void GameInstance::Execute()
{
    renderSystem.Execute();
}



