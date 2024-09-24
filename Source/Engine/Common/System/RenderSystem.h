
#ifndef INSTRENDERER_RENDERSYSTEM_H
#define INSTRENDERER_RENDERSYSTEM_H
#include "../../Render/RenderPassManager.h"
#include "../../Render/MaterialManager.h"
#include "../Scene.h"
#include "iostream"
struct RenderSysBeginInfo
{
    entt::registry* registry;
    GlobalDescriptorData globalData;
};


class RenderSystem
{
public:
    RenderSystem();
    void BeginSystem(RenderSysBeginInfo info);
    void Execute();

    void ResizeSwapChain();
    MaterialManager materialManager;
    RenderPassManager passManager;
};


#endif //INSTRENDERER_RENDERSYSTEM_H
