
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"
RenderSettings globalRenderSettingData{};

void RenderSystem::BeginSystem(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    InitSettings();
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
            renderSettingU->data = globalRenderSettingData.uniform;
        };
        uniArr.push_back(renderSettingU);
    }

    //CSM
    {

        auto csmU = INIT_UNIPTR(CSMUniform);
        csmU->Setup("CSMData", renderGraph);
        csmU->CustomInit = [=]()
        {


        };
        csmU->CustomUpdate = [=]()
        {
            csmU->data.width = CASCADED_WIDTH;
            csmU->data.height = CASCADED_HEIGHT;

            //auto viewCamera = std::make_shared<Camera>(scene->mainCamera);
            /*auto light = scene->mainLight;
            float factor = 2.0f;
            float camNear = scene->mainCamera.cameraData.nearPlane;
            float camFar = scene->mainCamera.cameraData.farPlane;
            float ratio = camFar/camNear;
            float fullLength = camFar - camNear;

            float scales[CASCADED_COUNT];
            for (int i = 0; i < CASCADED_COUNT; ++i)
            {
                float p = (i+1)/static_cast<float>(CASCADED_COUNT);
                float log = camNear * std::pow(ratio,p);
                float uniform = camNear + fullLength*p;
                float d = 0.95*(log-uniform)+uniform;
                scales[i] = (d-camNear)/fullLength;
            }

            float lastPlane = camNear;
            glm::mat4 vMat = scene->mainCamera.vpMat.view;
            glm::mat4 pMat = scene->mainCamera.vpMat.proj;*/
            /*for (int i = 0; i < CASCADED_COUNT; ++i)
            {
                float subFrustumNear = lastPlane;
                float offset = fullLength*scales[i];
                float subFrustumFar = camNear+offset;
                lastPlane = subFrustumFar;
                csmU->data.cascadeSplits[i] = glm::vec4((camNear+scales[i]*fullLength)*-1.0f);

                glm::vec4 clipNear = pMat*glm::vec4(0,0,-subFrustumNear,1.0f);
                float ndcNear = clipNear.z / clipNear.w;
                glm::vec4 clipFar = (pMat*glm::vec4(0,0,-subFrustumFar,1.0f));
                float ndcFar = clipFar.z / clipFar.w;

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
                    //wsCorner[j] =invMat*glm::vec4(ndcCorners[j],1.0);
                    auto t = invMat*glm::vec4(ndcCorners[j],1.0);
                    wsCorner[j] = glm::vec3(t/t.w);
                    wsCorner[j].z *=-1;
                }

                auto [sphereCenter,sphereRadius] = EngineMath::GetFrustumCircumsphere(wsCorner,subFrustumFar-subFrustumNear);
                auto [subFrustumVMat,subFrustumPMat] = light->GetSubFrustumLightMatrix(&scene->reg,sphereCenter,sphereRadius,scene->minPoint,scene->maxPoint);
                csmU->data.viewProjMat[i] = subFrustumPMat*subFrustumVMat;
            }*/
            //float cascadeSplits[CASCADED_COUNT];

            /*auto light = scene->mainLight;
            float factor = 2.0f;
            float camNear = scene->mainCamera.cameraData.nearPlane;
            float camFar = scene->mainCamera.cameraData.farPlane;
            float ratio = camFar/camNear;
            float fullLength = camFar - camNear;

            float scales[CASCADED_COUNT];
            for (int i = 0; i < CASCADED_COUNT; ++i)
            {
                float p = (i+1)/static_cast<float>(CASCADED_COUNT);
                float log = camNear * std::pow(ratio,p);
                float uniform = camNear + fullLength*p;
                float d = 0.95*(log-uniform)+uniform;
                scales[i] = (d-camNear)/fullLength;
            }*/

            glm::mat4 vMat = scene->mainCamera.vpMat.view;
            glm::mat4 pMat = scene->mainCamera.vpMat.proj;
            float cascadeSplits[CASCADED_COUNT];
            float nearClip = scene->mainCamera.cameraData.nearPlane;
            float farClip = scene->mainCamera.cameraData.farPlane;;
            float clipRange = farClip - nearClip;

            float minZ = nearClip;
            float maxZ = nearClip + clipRange;//farClip

            float range = maxZ - minZ;//farClip-nearClip
            float ratio = maxZ / minZ;//farClip/nearClip

            // Calculate split depths based on view camera frustum
            // Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
            for (uint32_t i = 0; i < CASCADED_COUNT; i++) {
                float p = (i + 1) / static_cast<float>(CASCADED_COUNT);
                float log = minZ * std::pow(ratio, p);
                float uniform = minZ + range * p;
                float d = 0.95 * (log - uniform) + uniform;
                cascadeSplits[i] = (d - nearClip) / clipRange;
            }

            // Calculate orthographic projection matrix for each cascade
            float lastSplitDist = 0.0;
            for (uint32_t i = 0; i < CASCADED_COUNT; i++) {
                float splitDist = cascadeSplits[i];

                glm::vec3 frustumCorners[8] = {
                        glm::vec3(-1.0f,  1.0f, 0.0f),
                        glm::vec3( 1.0f,  1.0f, 0.0f),
                        glm::vec3( 1.0f, -1.0f, 0.0f),
                        glm::vec3(-1.0f, -1.0f, 0.0f),
                        glm::vec3(-1.0f,  1.0f,  1.0f),
                        glm::vec3( 1.0f,  1.0f,  1.0f),
                        glm::vec3( 1.0f, -1.0f,  1.0f),
                        glm::vec3(-1.0f, -1.0f,  1.0f),
                };

                // Project frustum corners into world space
                glm::mat4 invCam = glm::inverse(pMat * vMat);
                for (uint32_t j = 0; j < 8; j++) {
                    glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[j], 1.0f);
                    frustumCorners[j] = invCorner / invCorner.w;
                }

                for (uint32_t j = 0; j < 4; j++) {
                    glm::vec3 dist = frustumCorners[j + 4] - frustumCorners[j];//
                    frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
                    frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
                }

                // Get frustum center
                glm::vec3 frustumCenter = glm::vec3(0.0f);
                for (uint32_t j = 0; j < 8; j++) {
                    frustumCenter += frustumCorners[j];
                }
                frustumCenter /= 8.0f;

                float radius = 0.0f;
                for (uint32_t j = 0; j < 8; j++) {
                    float distance = glm::length(frustumCorners[j] - frustumCenter);
                    radius = glm::max(radius, distance);
                }
                radius = std::ceil(radius * 16.0f) / 16.0f;

                glm::vec3 maxExtents = glm::vec3(radius);
                glm::vec3 minExtents = -maxExtents;
                auto mainLight = scene->mainLight;
                auto trans = scene->reg.get<Transform>(mainLight->entityID);
                auto rotationMat = EngineMath::GetRotateMatrix(trans.rotation);
                glm::vec4 target = rotationMat*glm::vec4(0.0,0.0,1.0,0.0);
                glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - glm::vec3(target) * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, (maxExtents.z - minExtents.z)*2);
                lightOrthoMatrix[1][1] *=-1;
                // Store split distance and matrix in cascade
                csmU->data.cascadeSplits[i] = glm::vec4((nearClip + splitDist * clipRange) * -1.0f);
                csmU->data.viewProjMat[i] = lightOrthoMatrix * lightViewMatrix;

                lastSplitDist = cascadeSplits[i];
            }

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

void RenderSystem::InitSettings()
{
    std::ifstream jsonFile(FILE_PATH("Asset/Json/RenderSetting.json"));
    nlohmann::json j;
    jsonFile>>j;
    globalRenderSettingData = settingSerializer.FromJson(j);
    auto& lightComp= scene->reg.get<LightComponent>(scene->mainLight->entityID);
    lightComp.Intensity = globalRenderSettingData.defaultLightIntensity;
    lightComp.range = globalRenderSettingData.defaultLightRange;
}







