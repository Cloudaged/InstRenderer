
#ifndef INSTRENDERER_RENDERSYSTEM_H
#define INSTRENDERER_RENDERSYSTEM_H
#include "../../Render/MaterialManager.h"
#include "../Scene/Scene.h"
#include "iostream"
#include "../../Render/Uniforms.h"
#include "../Core/EngineMath.h"
#include "../../Render/RenderGraph/RenderGraph.h"
class RenderSystem
{
public:
    RenderSystem();
    void BeginSystem(std::shared_ptr<Scene> scene);
    void Execute();

    //MaterialManager materialManager;
    //RenderPassManager passManager;
    void UpdateLightArray();
public:
    RDG::RenderGraph renderGraph;
private:
    void PrepareData();
    void PrepareLight();
    void PrepareGlobal();
    void MemoryCopy();
    void InitGlobalDescriptorSet();
    void SetupRenderGraph();
private:
    LightUniform lightUniform;
    GlobalUniform globalUniform;
    std::shared_ptr<Scene> scene;
    GlobalDescriptorData globalData{};
    //RDG::RenderGraph renderGraph;
};


#endif //INSTRENDERER_RENDERSYSTEM_H
