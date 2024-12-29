
#ifndef INSTRENDERER_RENDERSYSTEM_H
#define INSTRENDERER_RENDERSYSTEM_H
#include "../Scene/Scene.h"
#include "iostream"
#include "../../Render/Uniforms.h"
#include "../Core/EngineMath.h"
#include "../../Render/RenderGraph/RenderGraph.h"
#include "../../Render/UniContainer.h"
class RenderSystem
{
public:
    RenderSystem();
    void BeginSystem(std::shared_ptr<Scene> scene);
    void Execute();
    void UpdateLightArray();
public:
    RDG::RenderGraph renderGraph;
private:
    void PrepareData();
    void SetupRenderGraph();
    void SetupUniforms();
private:
    UniPtr<LightUniform> lightU;
    std::vector<std::shared_ptr<UniBase>> uniArr;
    std::shared_ptr<Scene> scene;
    //GlobalDescriptorData globalData{};
};


#endif //INSTRENDERER_RENDERSYSTEM_H
