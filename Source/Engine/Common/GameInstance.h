
#ifndef INSTRENDERER_GAMEINSTANCE_H
#define INSTRENDERER_GAMEINSTANCE_H
#include "iostream"
#include "Scene/Scene.h"
#include "SDL2/SDL.h"
#include "../Render/VulkanContext.h"
#include "System/RenderSystem.h"
#include "System/ResourceSystem.h"
#include "../Resource/ModelLoader.h"
#include "../Resource/ResTexture.h"
#include "../Resource/ImageLoader.h"
#include "Controller.h"
#include "Core/Locker.h"
#include "Core/WindowContext.h"
#include "../Render/Uniforms.h"
class GameInstance
{
public:
    explicit GameInstance(std::shared_ptr<WindowContext> windowContext);
    void Run();
public:
    std::shared_ptr<Scene> mainScene;
    std::shared_ptr<WindowContext> windowContext;
    SDL_Event event{};
    Controller controller;
    RenderSystem renderSystem;
    ResourceSystem resourceSystem;
private:
    void Tick();
    void InitCore();
    void InitSystem();
    void Update();
    void Execute();
    bool AllowToTick();
    void InitVulkanContext();
    void SDLEvent();
};


#endif //INSTRENDERER_GAMEINSTANCE_H
