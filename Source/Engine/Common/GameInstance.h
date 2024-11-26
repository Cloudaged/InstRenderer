
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

class GameInstance
{
public:
    GameInstance(WindowSize size);
    void Tick();
    void Run(bool* isClose);
public:
    Scene* mainScene;
    EntityManager* entityManager;
    SDL_Window* window = nullptr;
    SDL_Event event;
    WindowSize size;
    Controller* controller;
    bool isRun = true;
    bool isReady = false;
    RenderSystem renderSystem;
    ResourceSystem resourceSystem;
private:
    void InitCore();
    void InitSystem();
    void InitEntity();
    void InitWindow(WindowSize size);
    void InitVulkanContext();
    void SDLEvent();
};


#endif //INSTRENDERER_GAMEINSTANCE_H
