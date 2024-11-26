
#ifndef INSTRENDERER_GAMEINSTANCE_H
#define INSTRENDERER_GAMEINSTANCE_H
#include "iostream"
#include "Scene.h"
#include "Meta/Meta.h"
#include "SDL2/SDL.h"
#include "../Render/VulkanContext.h"
#include "System/RenderSystem.h"
#include "System/ResourceSystem.h"
#include "../Resource/ModelLoader.h"
#include "../Resource/ResTexture.h"
#include "../Resource/ImageLoader.h"
#include "Controller.h"
#include "Locker.h"
#include "Core/WindowContext.h"

class GameInstance
{
public:
    GameInstance(std::shared_ptr<WindowContext> windowContext);
    void Run();
public:
    std::shared_ptr<Scene> mainScene;
    SDL_Event event;
    std::shared_ptr<WindowContext> windowContext;
    Controller* controller;
    bool isRun = true;
    RenderSystem renderSystem;
    ResourceSystem resourceSystem;
private:
    void Tick();
    void InitCore();
    void InitSystem();
    void InitVulkanContext();
    void Update();
    void Execute();
    void ReceiveEvent();
    bool AllowToTick();
};


#endif //INSTRENDERER_GAMEINSTANCE_H
