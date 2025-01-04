
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"
RenderSettingUniform globalRenderSettingData{};

void RenderSystem::BeginSystem(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    SetupRenderGraph();
    for (auto& u:uniArr)
    {
        u->InitData();
    }
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
    //Global
    {
        auto globalU = INIT_UNIPTR(GlobalUniform);
        globalU->Setup("GlobalData", renderGraph);
        globalU->CustomUpdate = [=]()
        {
            globalU->data.skyboxProj = glm::perspective(glm::radians(80.0f),
                                                        scene->mainCamera.cameraData.aspect,
                                                        0.001f, 256.0f);
            globalU->data.skyboxProj[1][1] *= -1;
            globalU->data.view = scene->mainCamera.vpMat.view;
            globalU->data.proj = scene->mainCamera.vpMat.proj;

            auto [lightV, lightP] = scene->mainLight->GetLightMatrix(&scene->reg, scene->minPoint, scene->maxPoint);
            globalU->data.lightViewMat = lightV;
            globalU->data.lightProjMat = lightP;
        };
        uniArr.push_back(globalU);
    }

    //Light
    {
        this->lightU = INIT_UNIPTR(LightUniform);
        lightU->Setup("Lights", renderGraph);
        lightU->CustomInit = [=]()
        {
            UpdateLightArray();
        };
        lightU->CustomUpdate = [=]()
        {
            lightU->data.cameraPos = glm::vec4(scene->mainCamera.position, 1.0);
            lightU->data.cameraDir = glm::vec4(scene->mainCamera.viewPoint - scene->mainCamera.position, 1.0);
        };
        uniArr.push_back(lightU);
    }

    //RenderSetting
    {
        auto renderSettingU = INIT_UNIPTR(RenderSettingUniform);
        renderSettingU->Setup("RenderSettings", renderGraph);
        renderSettingU->CustomUpdate = [=]()
        {
            renderSettingU->data = globalRenderSettingData;
        };
        uniArr.push_back(renderSettingU);
    }

    //CSM
    {
        auto csmU = INIT_UNIPTR(CSMUniform);
        csmU->Setup("CSMData", renderGraph);
        csmU->CustomInit = [=]()
        {
            auto viewCamera = std::make_shared<Camera>(scene->mainCamera);
            auto light = scene->mainLight;

            std::vector<float> scales = {0.1, 0.3, 0.6, 1.0};
            float camNear = viewCamera->cameraData.nearPlane;
            float camFar = viewCamera->cameraData.farPlane;
            float fullLength = camFar - camNear;
            float offset = 0.0f;

            glm::mat4 vMat = viewCamera->vpMat.view;
            glm::mat4 pMat = viewCamera->vpMat.proj;
            for (int i = 0; i < CASCADED_COUNT; ++i)
            {
                float subFrustumNear = camNear+offset;
                offset += fullLength*scales[i];
                float subFrustumFar = camNear+offset;

                float ndcNear = (pMat*glm::vec4(0,0,subFrustumNear,1)).z;
                float ndcFar = (pMat*glm::vec4(0,0,subFrustumFar,1)).z;

                std::vector<glm::vec3> ndcCorners =
                        {
                                glm::vec3(-1.0,1.0,ndcNear),
                                glm::vec3(1.0,1.0,ndcNear),
                                glm::vec3(1.0,-1.0,ndcNear),
                                glm::vec3(-1.0,-1.0,ndcNear),

                                glm::vec3(-1.0,1.0,ndcFar),
                                glm::vec3(1.0,1.0,ndcFar),
                                glm::vec3(1.0,-1.0,ndcFar),
                                glm::vec3(-1.0,-1.0,ndcFar)
                        };

                auto invMat = glm::inverse(pMat*vMat);
                std::vector<glm::vec3> wsCorner(ndcCorners.size());
                for (int j = 0; j < ndcCorners.size(); ++j)
                {
                    wsCorner[i] = glm::vec3(invMat*glm::vec4(ndcCorners[i],1.0));
                }

                auto [sphereCenter,sphereRadius] = EngineMath::GetFrustumCircumsphere(wsCorner,fullLength);
                auto [subFrustumVMat,subFrustumPMat] = light->GetSubFrustumLightMatrix(&scene->reg,sphereCenter,sphereRadius,scene->minPoint,scene->maxPoint);

            }

        };
        csmU->CustomUpdate = [=]()
        {

        };
        uniArr.push_back(csmU);
    }
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







