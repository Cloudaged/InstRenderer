
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"
RenderSettingData globalRenderSettingData{};

void RenderSystem::BeginSystem(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    SetupRenderGraph();
    UpdateLightArray();
    //passManager.Setup(scene->reg.view<Renderable,Transform>(),globalData);
    //materialManager.Setup(passManager.geoPass->materialLayout,scene->reg.view<Renderable>());
    //VulkanContext::GetContext().passManager = &this->passManager;
}

void RenderSystem::Execute()
{
    PrepareData();
    VulkanContext::GetContext().DrawPrepare();
    renderGraph.Execute();
    VulkanContext::GetContext().Submit();
}

RenderSystem::RenderSystem()
{
    //passManager = RenderPassManager();
}

void RenderSystem::PrepareData()
{
    PrepareGlobal();
    PrepareLight();
    UniformCopy();
}

void RenderSystem::UpdateLightArray()
{
    int num=0;
    auto view= scene->reg.view<LightComponent,Transform>();
    for (auto& entityID:view)
    {
        auto& lightComp= view.get<LightComponent>(entityID);
        auto& trans = view.get<Transform>(entityID);
        glm::mat4 mat = EngineMath::GetRotateMatrix(trans.rotation);
        glm::vec4 rotatedDir = mat*glm::vec4(0.0,0.0,1.0,0.0);
        lightUniform.second.lights[num] = std::move(LightUnitsInShader{glm::vec4(trans.pos,1.0),rotatedDir,
                                                                glm::vec4(lightComp.color,1.0),
                                                                (int)lightComp.type,lightComp.Intensity,
                                                                glm::radians(lightComp.range),lightComp.lightSize,lightComp.attenuation});
        num++;
    }
    lightUniform.second.count=num;
}

void RenderSystem::PrepareLight()
{

    lightUniform.second.cameraPos= glm::vec4(scene->mainCamera.position,1.0);
    lightUniform.second.cameraDir = glm::vec4(scene->mainCamera.viewPoint-scene->mainCamera.position,1.0);
}

void RenderSystem::PrepareGlobal()
{
    globalUniform.second.skyboxProj = glm::perspective(glm::radians(80.0f),
                                                scene->mainCamera.cameraData.aspect,
                                                0.001f, 256.0f);
    globalUniform.second.skyboxProj[1][1] *=-1;
    globalUniform.second.view = scene->mainCamera.vpMat.view;
    globalUniform.second.proj = scene->mainCamera.vpMat.proj;

    auto [lightV,lightP] = scene->mainLight->GetLightMatrix(&scene->reg,scene->minPoint,scene->maxPoint);
    globalUniform.second.lightViewMat = lightV;
    globalUniform.second.lightProjMat = lightP;
}

void RenderSystem::UniformCopy()
{
    MemoryCopy(globalUniform);
    //MemoryCopy(lightUniform);
}

void RenderSystem::SetupRenderGraph()
{
    renderGraph.Compile(scene);
    VulkanContext::GetContext().presentManager.recreatePassFunc = std::bind(&RDG::RenderGraph::RecreateAllPass,&renderGraph);
    globalUniform.first = renderGraph.GetResourceHandle("GlobalData");
}

template<typename T>
void RenderSystem::MemoryCopy(std::pair<Handle, T> uniform)
{
    const RDG::ResourceRef& res = renderGraph.resourceMap[uniform.first];
    if(!res.bufferInfo.has_value())
        std::cout<<"copy to null address\n";
    memcpy(res.bufferInfo->mappedAddress,&uniform.second,sizeof(T));
}






