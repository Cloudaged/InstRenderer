
#ifndef INSTRENDERER_RENDERSYSTEM_H
#define INSTRENDERER_RENDERSYSTEM_H
#include "../../Render/RenderPassManager.h"
#include "../../Render/MaterialManager.h"
#include "../Scene/Scene.h"
#include "iostream"
#include "../../Render/RenderGraph/RenderGraph.h"
#include "../../Render/Uniforms.h"


class RenderSystem
{
public:
    RenderSystem();
    void BeginSystem(std::shared_ptr<Scene> scene);
    void Execute();

    MaterialManager materialManager;
    RenderPassManager passManager;
    void UpdateLightArray();
private:
    LightUniform lightUniform;
    GlobalUniform globalUniform;

    void PrepareData();
    void PrepareLight();
    void PrepareGlobal();
    void MemoryCopy();
    std::shared_ptr<Scene> scene;
    void InitGlobalDescriptorSet();
    GlobalDescriptorData globalData{};
    void SetupRenderGraph();
    //RDG::RenderGraph renderGraph;
};


#endif //INSTRENDERER_RENDERSYSTEM_H
