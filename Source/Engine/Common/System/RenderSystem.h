
#ifndef INSTRENDERER_RENDERSYSTEM_H
#define INSTRENDERER_RENDERSYSTEM_H
#include "../../Render/RenderPassManager.h"
#include "../../Render/MaterialManager.h"
#include "../Scene/Scene.h"
#include "iostream"
#include "../../Render/RenderGraph/RenderGraph.h"
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

    MaterialManager materialManager;
    RenderPassManager passManager;
private:
    void SetupRenderGraph();
    //RDG::RenderGraph renderGraph;
};


#endif //INSTRENDERER_RENDERSYSTEM_H
