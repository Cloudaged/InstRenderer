
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

class GameInstance
{
public:
    GameInstance(WindowSize size);
    Scene* mainScene;
    void Tick();

    EntityManager* entityManager;
    //entt::registry reg;
    SDL_Window* window = nullptr;
private:
    RenderSystem renderSystem;
    void InitCore();
    void InitSystem();
    void InitEntity();
    void InitWindow(WindowSize size);
    void InitVulkanContext();
};


#endif //INSTRENDERER_GAMEINSTANCE_H
