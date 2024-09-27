
#ifndef INSTRENDERER_GAMEINSTANCE_H
#define INSTRENDERER_GAMEINSTANCE_H
#include "iostream"
#include "GameType.h"
#include "Scene.h"
#include "Meta/Meta.h"
#include "SDL2/SDL.h"

#include "../Render/VulkanContext.h"
#include "System/RenderSystem.h"
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
    Scene* mainScene;
    void Tick();
    void Run(bool* isClose);
    EntityManager* entityManager;
    //entt::registry reg;
    SDL_Window* window = nullptr;
    SDL_Event event;
    WindowSize size;
    Controller* controller;
    bool isRun = true;
    bool isReady = false;

private:
    RenderSystem renderSystem;
    void InitCore();
    void InitSystem();
    void InitEntity();
    void InitWindow(WindowSize size);
    void InitVulkanContext();
    void SDLEvent();
};


#endif //INSTRENDERER_GAMEINSTANCE_H
