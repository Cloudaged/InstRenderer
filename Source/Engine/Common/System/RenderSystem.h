
#ifndef INSTRENDERER_RENDERSYSTEM_H
#define INSTRENDERER_RENDERSYSTEM_H
#include "../Scene/Scene.h"
#include "iostream"
#include "random"
#include "../../Render/Uniforms.h"
#include "../Core/EngineMath.h"
#include "../../Render/RenderGraph/RenderGraph.h"
#include "../../Render/RenderGraph/RenderResource.h"

#include "../../Render/UniContainer.h"
#include "../Serialization/RenderSettingSerializer.h"
#include "fstream"
#include "../Core/PathDefine.h"
struct RenderSettings
{
    RenderSettingUniform uniform;
    float defaultLightIntensity;
    float defaultLightRange;
};

class RenderSystem
{
public:
    RenderSystem();
    void BeginSystem(std::shared_ptr<Scene> scene);
    void Execute();
    void UpdateLightArray();
    void RecreateRTScene();
public:
    RDG::RenderGraph rg;
private:
    void DeclareResource();
    void PrepareData();
    void SetupRenderGraph();
    void SetupUniforms();
    void InitSettings();
private:
    std::shared_ptr<UniContainer<GeometryNodeArr>> materialArr;
    RenderSettingSerializer settingSerializer;
    UniPtr<LightUniform> lightU;
    std::vector<std::shared_ptr<UniBase>> uniArr;
    std::shared_ptr<Scene> scene;
    entt::view<entt::get_t<Renderable,Transform>> view;
    //GlobalDescriptorData globalData{};
};


#endif //INSTRENDERER_RENDERSYSTEM_H
