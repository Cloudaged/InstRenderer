
#ifndef INSTRENDERER_GAMEINSTANCE_H
#define INSTRENDERER_GAMEINSTANCE_H
#include "iostream"
#include "GameType.h"
#include "Scene.h"
#include "Meta/Meta.h"
#include "SDL2/SDL.h"
#include "../Render/VulkanContext.h"
#include "System/RenderSystem.h"
#include "System/ResourceSystem.h"
#include "EntityManager.h"
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
    Scene* mainScene;
    EntityManager* entityManager;
    SDL_Event event;
    std::shared_ptr<WindowContext> windowContext;
    Controller* controller;
    bool isRun = true;
    bool isReady = false;
    RenderSystem renderSystem;
    ResourceSystem resourceSystem;
private:
    void Tick();
    void InitCore();
    void InitSystem();
    void InitEntity();
    void InitVulkanContext();
    void SDLEvent();
};


#endif //INSTRENDERER_GAMEINSTANCE_H
