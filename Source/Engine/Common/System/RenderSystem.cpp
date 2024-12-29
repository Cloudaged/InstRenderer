
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"
RenderSettingUniform globalRenderSettingData{};

void RenderSystem::BeginSystem(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    SetupRenderGraph();
    UpdateLightArray();
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
}

void RenderSystem::SetupUniforms()
{
    auto globalU = INIT_UNIPTR(GlobalUniform);
    globalU->Setup("GlobalData",renderGraph);
    globalU->CustomUpdate = [=](){
        globalU->data.skyboxProj = glm::perspective(glm::radians(80.0f),
                                                   scene->mainCamera.cameraData.aspect,
                                                   0.001f, 256.0f);
        globalU->data.skyboxProj[1][1] *=-1;
        globalU->data.view = scene->mainCamera.vpMat.view;
        globalU->data.proj = scene->mainCamera.vpMat.proj;

        auto [lightV,lightP] = scene->mainLight->GetLightMatrix(&scene->reg,scene->minPoint,scene->maxPoint);
        globalU->data.lightViewMat = lightV;
        globalU->data.lightProjMat = lightP;
    };
    uniArr.push_back(globalU);

    this->lightU = INIT_UNIPTR(LightUniform);
    lightU->Setup("Lights",renderGraph);
    lightU->CustomUpdate = [=]()
    {
        lightU->data.cameraPos= glm::vec4(scene->mainCamera.position,1.0);
        lightU->data.cameraDir = glm::vec4(scene->mainCamera.viewPoint-scene->mainCamera.position,1.0);
    };
    uniArr.push_back(lightU);

    auto renderSettingU = INIT_UNIPTR(RenderSettingUniform);
    renderSettingU->Setup("RenderSettings",renderGraph);
    renderSettingU->CustomUpdate = [=]()
    {
        renderSettingU->data = globalRenderSettingData;
    };
}



void RenderSystem::PrepareData()
{
    for (auto& uni:uniArr)
    {
        uni->UpdateData();
    }
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
        lightU->data.lights[num] = std::move(LightUnitsInShader{glm::vec4(trans.pos,1.0),rotatedDir,
                                                                glm::vec4(lightComp.color,1.0),
                                                                (int)lightComp.type,lightComp.Intensity,
                                                                glm::radians(lightComp.range),lightComp.lightSize,lightComp.attenuation});
        num++;
    }
    lightU->data.count=num;
}




void RenderSystem::SetupRenderGraph()
{
    renderGraph.Compile(scene);
    VulkanContext::GetContext().presentManager.recreatePassFunc = std::bind(&RDG::RenderGraph::RecreateAllPass,&renderGraph);
    SetupUniforms();
}

void RenderSystem::PrepareRenderSetting()
{

}







