
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"
RenderSettings globalRenderSettingData{};

void RenderSystem::BeginSystem(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    this->view = scene->reg.view<Renderable,Transform>();
    InitSettings();
    SetupRenderGraph();
}

void RenderSystem::Execute()
{
    PrepareData();
    VulkanContext::GetContext().DrawPrepare();
    rg.Execute();
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
        globalU->Setup("GlobalData", rg);
        globalU->CustomInit = [=]()
        {
            globalU->data.nearPlane = scene->mainCamera.cameraData.nearPlane;
            globalU->data.farPlane = scene->mainCamera.cameraData.farPlane;
        };
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
        lightU->Setup("Lights", rg);
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
        renderSettingU->Setup("RenderSettings", rg);
        renderSettingU->CustomUpdate = [=]()
        {
            renderSettingU->data = globalRenderSettingData.uniform;
        };
        uniArr.push_back(renderSettingU);
    }

    //CSM
    {

        auto csmU = INIT_UNIPTR(CSMUniform);
        csmU->Setup("CSMData", rg);
        csmU->CustomInit = [=]()
        {


        };
        csmU->CustomUpdate = [=]()
        {
            csmU->data.width = CASCADED_WIDTH;
            csmU->data.height = CASCADED_HEIGHT;

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

            auto mainLight = scene->mainLight;
            auto lightTrans = scene->reg.get<Transform>(mainLight->entityID);
            auto rotationMat = EngineMath::GetRotateMatrix(lightTrans.rotation);
            glm::vec4 target = rotationMat*glm::vec4(0.0,0.0,1.0,0.0);
            auto shadowView = glm::lookAt(glm::vec3(0,0,0),glm::vec3(target),glm::vec3(0,1,0));

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

                std::vector<glm::vec3> frustumCorners = {
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

                auto [frustumCenter,radius] = EngineMath::GetFrustumCircumsphere(frustumCorners);
                csmU->data.radiusBias = 30.0f;
                radius +=csmU->data.radiusBias;

                float unitPerPix = radius*2.0/CASCADED_WIDTH;
                if(globalRenderSettingData.uniform.shadowDebug.antiShimmering==1)
                {
                    auto fcLS = glm::vec3(shadowView * glm::vec4(frustumCenter, 1.0));
                    fcLS -= glm::vec3(fmodf(fcLS.x, unitPerPix), fmodf(fcLS.y, unitPerPix), 0.0);
                    frustumCenter = glm::vec3(glm::inverse(shadowView) * glm::vec4(fcLS, 1.0));
                }
                csmU->data.unitPerPix[i] = glm::vec4(unitPerPix);

                auto [subVMat,subPMat] = mainLight->GetSubFrustumLightMatrix(&scene->reg,frustumCenter,radius,scene->minPoint,scene->maxPoint);
                // Store split distance and matrix in cascade
                csmU->data.cascadeSplits[i] = glm::vec4(frustumCenter,radius);
                csmU->data.viewProjMat[i] = subPMat * subVMat;

                lastSplitDist = cascadeSplits[i];
            }

        };
        uniArr.push_back(csmU);
    }

    //RTImage
    {
        auto rtU = INIT_UNIPTR(RTUniform);
        rtU->Setup("RTUniform", rg);
        rtU->CustomInit = [=]()
        {
            rtU->data.invView = glm::inverse(scene->mainCamera.vpMat.view);
            rtU->data.invProj = glm::inverse(scene->mainCamera.vpMat.proj);
        };
        rtU->CustomUpdate = [=]()
        {
            const Camera& mainCamera = scene->mainCamera;
            rtU->data.invView = glm::inverse(mainCamera.vpMat.view);
            rtU->data.invProj = glm::inverse(mainCamera.vpMat.proj);
            rtU->data.camUp = glm::vec4(mainCamera.upDir,0);
            rtU->data.camRight = glm::vec4(mainCamera.rightDir,0);
        };
        uniArr.push_back(rtU);
    }

    {
        materialArr = INIT_UNIPTR(GeometryNodeArr);
        materialArr->Setup("GeometryNodeArray", rg);
        materialArr->CustomInit = [=]()
        {

        };
        materialArr->CustomUpdate = [=]()
        {

        };
        uniArr.push_back(materialArr);
    }

    {
        auto kernels = INIT_UNIPTR(SSAOKernels);
        kernels->Setup("SSAOKernel", rg);
        kernels->CustomInit = [=]()
        {
            std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            auto Lerp = [](float a, float b, float f)
            { return a + f * (b - a); };
            for (int i = 0; i < 32; ++i)
            {
                glm::vec4 sample(randomFloats(generator) * 2.0 - 1.0,
                                 randomFloats(generator) * 2.0 - 1.0,
                                 randomFloats(generator),
                                 0
                );
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);

                float scale = float(i) / 32.0;

                scale = Lerp(0.1f, 1.0f, scale * scale);

                sample *= scale;

                kernels->data.kernels[i] = sample;
            }
        };
        kernels->CustomUpdate = [=]()
        {

        };
        uniArr.push_back(kernels);

    }

    {
        auto handle = rg.GetResourceHandle("SSAORotation");
        auto& rotationRes = rg.resourceMap[handle];


        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<glm::vec2> ssaoNoise;
        for (int i = 0; i < 16; ++i)
        {
            glm::vec2 noise(
                    randomFloats(generator)*2.0-1.0,
                    randomFloats(generator)*2.0-1.0
            );
            ssaoNoise.push_back(noise);
        }
        auto allocIMG = std::make_shared<AllocatedImage>(ImageType::Color,VK_FORMAT_R16G16_UNORM,
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,
                                VkExtent2D{SSAO_ROTATION_SIZE,SSAO_ROTATION_SIZE},1,1);
        allocIMG->LoadData(ssaoNoise.data(),ssaoNoise.size()*sizeof(glm::vec2));
        rotationRes.textureInfo->data = std::make_shared<Texture>(allocIMG,TextureType::DontCare);
    }

    {
        probeArea = INIT_UNIPTR(ProbeArea);
        probeArea->Setup("ProbeArea", rg);
        probeArea->CustomInit = [=]()
        {
            UpdateProbeArea();
        };
        probeArea->CustomUpdate = [=]()
        {

        };
        uniArr.push_back(probeArea);
    }

    {

        auto boxModel = ModelLoader::Load(FILE_PATH("Asset/Model/probe/probe.gltf"));
        auto& rawMesh = boxModel->rootNode->meshes[0];
        visualProbe = std::make_shared<Mesh>(rawMesh->verts,rawMesh->index);

    }

}

void RenderSystem::UpdateProbeArea()
{
    auto GetArrayIndex3D = [&](int x,int y,int z)
    {
        return x+PROBE_AREA_SIZE*y+z*PROBE_AREA_SIZE*PROBE_AREA_SIZE;
    };

    auto sceneMin = scene->minPoint;
    auto sceneMax = scene->maxPoint;

    float xStride = (sceneMax.x-sceneMin.x)/(float)PROBE_AREA_SIZE;
    float yStride = (sceneMax.y-sceneMin.y)/(float)PROBE_AREA_SIZE;
    float zStride = (sceneMax.z-sceneMin.z)/(float)PROBE_AREA_SIZE;

    auto& probes = probeArea->data.probes;
    size_t probeCount = PROBE_AREA_SIZE*PROBE_AREA_SIZE*PROBE_AREA_SIZE;

    for (int z = 0; z < PROBE_AREA_SIZE; ++z)
    {
        for (int y = 0; y < PROBE_AREA_SIZE; ++y)
        {
            for (int x = 0; x < PROBE_AREA_SIZE; ++x)
            {
                auto index = GetArrayIndex3D(x,y,z);
                probes[index].position =glm::vec4(sceneMin,0.0)+glm::vec4(x*xStride,y*yStride,z*zStride,1.0);
                for (int n = 0; n < PROBE_NORMAL_COUNT; ++n)
                {
                    //Fibonacci grid
                    float phi = 0.618;
                    auto& normal = probes[index].normals[n];
                    float zn = std::clamp((2*n-1)/(float)PROBE_NORMAL_COUNT-1,-1.0f,1.0f);
                    float xn = sqrt(1-zn*zn)* cos(2*PI*n*phi);
                    float yn = sqrt(1-zn*zn)* sin(2*PI*n*phi);
                    normal = glm::vec4(xn,yn,zn,0.0);
                }
            }
        }
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
    DeclareResource();
    SetupUniforms();
    for (auto& u:uniArr)
    {
        u->InitData();
    }
    rg.Compile();
    VulkanContext::GetContext().presentManager.recreatePassFunc = std::bind(&RDG::RenderGraph::RecreateAllPass,&rg);
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

void RenderSystem::RecreateRTScene()
{
    if(scene->rtScene.topAS.accelerationStructure!=VK_NULL_HANDLE)
    {
        std::cout<<"Exist\n";
    }else
    {
        std::cout<<"dontExist\n";
    }
    memcpy(materialArr->data.nodes,scene->nodeArr.data(),sizeof(GeometryNode)*300);
    scene->rtScene = RTBuilder::CreateRTScene(scene->reg.view<Renderable,Transform>());
    rg.accelerationStructure->rtScene = std::make_shared<RTScene>(scene->rtScene);
    rg.WriteAccelerationSTDescriptor(*rg.accelerationStructure);
}

void RenderSystem::DeclareResource()
{
    using namespace RDG;
    uint32_t winWidth = (uint32_t)VulkanContext::GetContext().windowExtent.width;
    uint32_t winHeight = (uint32_t)VulkanContext::GetContext().windowExtent.height;
    auto sceneSkybox = scene->skybox;
    auto globalData = rg.AddResource({.name = "GlobalData",.type = ResourceType::Uniform,
                                          .bufferInfo = BufferInfo{.size = sizeof(GlobalUniform)}});

    auto csmData = rg.AddResource({.name = "CSMData",.type = ResourceType::Uniform,
                                       .bufferInfo = BufferInfo{.size = sizeof(CSMUniform)}});

    auto skyboxTex = rg.AddResource({"SkyboxTexture",.type = ResourceType::Texture,
                                         .textureInfo = TextureInfo{{sceneSkybox->width,sceneSkybox->height},
                                                                    TextureUsage::ColorAttachment, VK_FORMAT_R8G8B8A8_SRGB, sceneSkybox->texture}});



    auto ssaoKernel = rg.AddResource({"SSAOKernel",.type = ResourceType::Uniform,
                                          .bufferInfo = BufferInfo{.size = sizeof(SSAOKernels)}});
    auto ssaoRotation = rg.AddResource({"SSAORotation",.type = ResourceType::Texture,
                                                .textureInfo = TextureInfo{{SSAO_ROTATION_SIZE,SSAO_ROTATION_SIZE},
                                                                           TextureUsage::ColorAttachment, VK_FORMAT_R16G16_UNORM}});

     auto cascadedShadowMapData = rg.AddResource({"CascadedShadowMapData",.type = ResourceType::Uniform,
                                                      .bufferInfo = BufferInfo{.size = sizeof(CSMUniform)}});

    auto cascadedShadowMap = rg.AddResource({"CascadedShadowMap",.type = ResourceType::Texture,
                                                 .textureInfo = TextureInfo{{CASCADED_WIDTH, CASCADED_HEIGHT},
                                                                            TextureUsage::ShadowMap, VK_FORMAT_D32_SFLOAT, nullptr, 1, CASCADED_COUNT}});

    auto lightData = rg.AddResource({.name = "Lights",.type = ResourceType::Uniform,
                                         .bufferInfo = BufferInfo{.size = sizeof(LightUniform)}});

    auto renderSettings = rg.AddResource({.name = "RenderSettings",.type = ResourceType::Uniform,
                                              .bufferInfo = BufferInfo{.size = sizeof(RenderSettingUniform)}});

    auto position = rg.AddResource({.name = "Position",.type = ResourceType::Texture,
                                        .textureInfo =TextureInfo{WINDOW_EXTENT,
                                                                  TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto viewPosition = rg.AddResource({"ViewPosition",.type = ResourceType::Texture,
                                               .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                          TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto viewNormal = rg.AddResource({"ViewNormal",.type = ResourceType::Texture,
                                                .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                           TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto normal = rg.AddResource({.name = "Normal",.type = ResourceType::Texture,
                                      .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                 TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto baseColor = rg.AddResource({.name = "BaseColor",.type = ResourceType::Texture,
                                         .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                    TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto metallicRoughness = rg.AddResource({.name = "MetallicRoughness",.type = ResourceType::Texture,
                                                 .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                            TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto depth = rg.AddResource({.name = "Depth",.type = ResourceType::Texture,
                                     .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                TextureUsage::Depth, VK_FORMAT_D32_SFLOAT}});

    auto ssaoOutput = rg.AddResource({.name = "SSAOOutput",.type = ResourceType::Texture,
                                          .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                     TextureUsage::ColorAttachment|TextureUsage::TransferDst,
                                                                     VK_FORMAT_R8_UNORM}});

    auto ssaoBlurIMG = rg.AddResource({.name = "SSAOBlur",.type = ResourceType::StorageImage,
                                            .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                       TextureUsage::Storage|TextureUsage::TransferSrc,
                                                                       VK_FORMAT_R8_UNORM}});

    auto lighted = rg.AddResource({.name = "Lighted",.type = ResourceType::Texture,
                                       .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                  TextureUsage::ColorAttachment, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto rtIMG  = rg.AddResource({.name = "RTIMG",.type = ResourceType::StorageImage,
                                      .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                 TextureUsage::Storage|TextureUsage::TransferSrc, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto rtSampleImg  = rg.AddResource({.name = "rtSampleImg",.type = ResourceType::Texture,
                                            .textureInfo = TextureInfo{WINDOW_EXTENT,
                                                                       TextureUsage::ColorAttachment|TextureUsage::TransferDst, VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto tlasData = rg.AddResource({.name = "TLAS",.type = ResourceType::Accleration,
                                        .rtScene = std::make_shared<RTScene>(scene->rtScene)});

    auto rtUniform = rg.AddResource({.name = "RTUniform",.type = ResourceType::Uniform,
                                         .bufferInfo = BufferInfo{.size = sizeof(RTUniform)}});



    auto ddgiIrradianceVolume = rg.AddResource({.name = "IrradianceVolume",.type = ResourceType::Texture,
                                                .textureInfo = TextureInfo{TextureExtent{PROBE_AREA_SIZE*PROBE_AREA_SIZE*6,PROBE_AREA_SIZE*6},
                                                                           TextureUsage::ColorAttachment,VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto ddgiDepthVolume = rg.AddResource({.name = "DepthVolume",.type = ResourceType::Texture,
                                                       .textureInfo = TextureInfo{TextureExtent{PROBE_AREA_SIZE*PROBE_AREA_SIZE*14,PROBE_AREA_SIZE*14},
                                                                                  TextureUsage::ColorAttachment,VK_FORMAT_R16G16_SFLOAT}});

    auto ddgiRadianceRaySamples = rg.AddResource({.name = "RadianceSample",.type = ResourceType::StorageImage,
                                                         .textureInfo = TextureInfo{TextureExtent{PROBE_AREA_SIZE*PROBE_AREA_SIZE*PROBE_AREA_SIZE,RAYS_PER_PROBE},
                                                                                    TextureUsage::Storage,VK_FORMAT_R16G16B16A16_SFLOAT}});

    auto ddgiDepthRaySamples = rg.AddResource({.name = "DepthSample",.type = ResourceType::StorageImage,
                                                         .textureInfo = TextureInfo{TextureExtent{PROBE_AREA_SIZE*PROBE_AREA_SIZE*PROBE_AREA_SIZE,RAYS_PER_PROBE},
                                                                                    TextureUsage::Storage,VK_FORMAT_R16G16_SFLOAT}});

    auto ddgiProbesArea = rg.AddResource({"ProbeArea",.type=ResourceType::SSBO,
                                            .bufferInfo = {BufferInfo{.size = sizeof(ProbeArea)}}});

    auto nodeArray = rg.AddResource({"GeometryNodeArray",.type=ResourceType::SSBO,
                                             .bufferInfo = {BufferInfo{.size = 300*sizeof(GeometryNode)}}});
    //Pass
    //GeoPass
    {
        struct alignas(16) GeoPC
        {
            Handle global;
            Material mat;
            glm::mat4 model;
        };

        rg.AddPass({.name = "Geometry",.type = RenderPassType::Raster,.fbExtent = WINDOW_EXTENT,
                        .input = {globalData},
                        .output = {position, normal, baseColor, metallicRoughness,viewPosition,viewNormal, depth},
                        .pipeline = {.type=PipelineType::Mesh, .rsShaders={"GeoVert", "GeoFrag"},.handleSize = sizeof(GeoPC)},
                        .executeFunc = [=](CommandList& cmd)
                        {
                            for (auto& entity:view)
                            {
                                auto renderComp = view.get<Renderable>(entity);
                                auto transComp = view.get<Transform>(entity);
                                GeoPC pushConstants = {globalData,renderComp.material,transComp.globalTransform};
                                cmd.PushConstantsForHandles(&pushConstants);
                                cmd.DrawMesh(renderComp.mesh);
                            }
                        }});
    }

    {
        struct alignas(16) csmPC
        {
            glm::mat4 modelMat;
            Handle csmUniform;
        };

        rg.AddPass({.name = "csmPass",.type = RenderPassType::Raster,.fbExtent = {CASCADED_WIDTH, CASCADED_HEIGHT},
                        .input = {csmData},
                        .output = {cascadedShadowMap},
                        .pipeline = {.type=PipelineType::Mesh, .rsShaders={"CascadedShadowVert","CascadedShadowFrag"}, .handleSize = sizeof(csmPC)},
                        .executeFunc = [=](CommandList& cmd)
                        {
                            for (auto& entity:view)
                            {
                                auto renderComp = view.get<Renderable>(entity);
                                auto transComp = view.get<Transform>(entity);
                                csmPC pushConstants = {transComp.globalTransform,csmData};
                                cmd.PushConstantsForHandles(&pushConstants);
                                cmd.DrawMesh(renderComp.mesh);
                            }
                        }});


    }

    {
        struct alignas(16) SSAOPC
        {
            Handle position;
            Handle normal;
            Handle ssaoKernel;
            Handle ssaoRotation;
            Handle globalData;
        };
        rg.AddPass({.name = "SSAO",.type = RenderPassType::Raster,.fbExtent = WINDOW_EXTENT,
                           .input = {viewPosition,viewNormal,ssaoKernel,ssaoRotation,globalData},
                           .output = {ssaoOutput},
                           .pipeline = {.type = PipelineType::RenderQuad, .rsShaders = {"SSAOVert", "SSAOFrag"},.handleSize = sizeof(SSAOPC)},
                           .executeFunc = [=](CommandList& cmd)
                           {
                               SSAOPC pushConstants = {viewPosition,viewNormal,ssaoKernel,ssaoRotation,globalData};
                               cmd.PushConstantsForHandles(&pushConstants);
                               cmd.DrawRenderQuad();
                           }});
    }

    {
        struct alignas(16) SSAOBlurPC
        {
            Handle ssaoBlurIMG;
            Handle ssaoOutput;
            int xBlurRange;
            int yBlurRange;
        };
        rg.AddPass({.name = "SSAOBlur",.type = RenderPassType::Compute,.fbExtent = WINDOW_EXTENT,
                           .input = {ssaoBlurIMG,ssaoOutput},
                           .output = {ssaoBlurIMG},
                           .pipeline = {.type = PipelineType::Compute, .cpShaders = {"SSAOBlur"},.handleSize = sizeof(SSAOBlurPC)},
                           .executeFunc = [=](CommandList& cmd)
                           {
                               SSAOBlurPC pushConstants = {ssaoBlurIMG,ssaoOutput,5,0};
                               cmd.PushConstantsForHandles(&pushConstants);
                               cmd.Dispatch();
                               pushConstants = {ssaoBlurIMG,ssaoOutput,0,5};
                               cmd.PushConstantsForHandles(&pushConstants);
                               cmd.Dispatch();
                               cmd.TransImage(rg.resourceMap[ssaoBlurIMG].textureInfo.value(),rg.resourceMap[ssaoOutput].textureInfo.value(),
                                              VK_IMAGE_LAYOUT_GENERAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
                           }});
    }

    {
        struct alignas(16) RTRadiancePC
        {
            Handle radianceMap;
            Handle depthMap;
            Handle tlas;
            Handle geometryNodeArray;
            Handle lightData;
            Handle skyboxTex;
            Handle probesArea;
            Handle rtUniform;
        };

        rg.AddPass({.name = "RTRadiancePass",.type = RenderPassType::RayTracing,.fbExtent = TextureExtent{PROBE_AREA_SIZE*PROBE_AREA_SIZE*PROBE_AREA_SIZE,RAYS_PER_PROBE},
                           .input = {ddgiRadianceRaySamples,ddgiDepthRaySamples,tlasData,nodeArray,lightData,skyboxTex,ddgiProbesArea,rtUniform},
                           .output = {ddgiRadianceRaySamples,ddgiDepthRaySamples},
                           .pipeline = {.type = PipelineType::RayTracing,
                                   .rtShaders ={.chit = "DDGIClosestHit",.gen = "DDGIGen",.miss = "DDGIMiss",.miss_shadow = "DDGIShadowMiss",.ahit ="anyHit"},
                                   .handleSize = sizeof(RTRadiancePC)},
                           .executeFunc = [=](CommandList& cmd)
                           {
                               RTRadiancePC radiancePc = {ddgiRadianceRaySamples,ddgiDepthRaySamples,tlasData,nodeArray,lightData,skyboxTex,ddgiProbesArea,rtUniform};
                               cmd.PushConstantsForHandles(&radiancePc);
                               cmd.RayTracing();
                           }});

    }

    {
        struct IrradianceVolumePC
        {
            Handle radianceMap;
            Handle depthMap;
            Handle irradianceVolume;
            Handle depthVolume;
        };

        rg.AddPass({.name = "IrradianceVolumePass",.type = RenderPassType::Compute,.fbExtent = TextureExtent{PROBE_AREA_SIZE*PROBE_AREA_SIZE,PROBE_AREA_SIZE},
                           .input = {ddgiRadianceRaySamples,ddgiDepthRaySamples,ddgiIrradianceVolume,ddgiDepthVolume},
                           .output = {ddgiIrradianceVolume,ddgiDepthVolume},
                           .pipeline = {.type = PipelineType::Compute, .cpShaders = {"DDGIVolume"},.handleSize = sizeof(IrradianceVolumePC)},
                           .executeFunc = [=](CommandList& cmd)
                           {
                               IrradianceVolumePC pushConstants = {ddgiRadianceRaySamples,ddgiDepthRaySamples,ddgiIrradianceVolume,ddgiDepthVolume};
                               cmd.PushConstantsForHandles(&pushConstants);
                               cmd.Dispatch();
                               /*cmd.TransImage(rg.resourceMap[ssaoBlurIMG].textureInfo.value(),rg.resourceMap[ssaoOutput].textureInfo.value(),
                                              VK_IMAGE_LAYOUT_GENERAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);*/
                           }});

    }


    //Light
    {
        struct alignas(16) CompPC
        {
            Handle position;
            Handle normal;
            Handle baseColor;
            Handle mr;
            Handle shadowMap;
            Handle csmData;
            Handle lightUniform;
            Handle globalUniform;
            Handle renderSettingUniform;
            Handle ssao;
        };

        rg.AddPass({.name = "Composition",.type = RenderPassType::Raster,.fbExtent = WINDOW_EXTENT,
                        .input = {position,normal,baseColor,metallicRoughness,cascadedShadowMap,csmData,globalData,lightData,renderSettings,ssaoOutput},
                        .output = {lighted},
                        .pipeline = {.type = PipelineType::RenderQuad, .rsShaders = {"CompVert", "CompFrag"},.handleSize = sizeof(CompPC)},
                        .executeFunc = [=](CommandList& cmd)
                        {
                            CompPC pushConstants = {position,normal,baseColor,metallicRoughness,cascadedShadowMap,csmData,lightData,globalData,renderSettings,ssaoOutput};
                            cmd.PushConstantsForHandles(&pushConstants);
                            cmd.DrawRenderQuad();
                        }});


    }

    {
        {
            struct ProbeVisualPC
            {
                Handle ProbeArea;
                Handle globalUniform;
            };

            rg.AddPass({.name = "ProbeVisual", .type = RenderPassType::Raster, .fbExtent = WINDOW_EXTENT,
                               .input = {ddgiProbesArea,globalData,lighted,depth},
                               .output = {lighted,depth},
                               .pipeline = {.type = PipelineType::Mesh, .rsShaders = {.vert = "DDGIProbeVisualVert",.frag = "DDGIProbeVisualFrag",}, .handleSize = sizeof(ProbeVisualPC)},
                               .executeFunc = [=](CommandList &cmd)
                               {
                                   ProbeVisualPC pushConstant = {ddgiProbesArea,globalData};
                                   cmd.PushConstantsForHandles(&pushConstant);
                                   cmd.DrawInstances(*visualProbe,PROBE_AREA_SIZE*PROBE_AREA_SIZE*PROBE_AREA_SIZE);
                               }
                       });
        }
    }

    {
        struct alignas(16) SkyboxPC
        {
            Handle skyboxTex;
            Handle globalData;
        };

        rg.AddPass({.name = "SkyboxDraw", .type = RenderPassType::Raster, .fbExtent = WINDOW_EXTENT,
                        .input = {skyboxTex,lighted,depth},
                        .output = {lighted,depth},
                        .pipeline = {.type = PipelineType::Skybox, .rsShaders = {"SkyboxVert", "SkyboxFrag"},.handleSize = sizeof(SkyboxPC)},
                        .executeFunc = [=](CommandList& cmd)
                        {
                            SkyboxPC pushConstant = {skyboxTex,globalData};
                            cmd.PushConstantsForHandles(&pushConstant);
                            cmd.DrawMesh(*sceneSkybox->mesh);
                        }
                });
    }
//    {
//        struct alignas(16) RTPC
//        {
//            Handle outputImg;
//            Handle tlas;
//            Handle rtUniform;
//            Handle geometryNodeArray;
//            Handle lightData;
//            Handle skyboxTex;
//        };
//
//        rg.AddPass({.name = "RayTracing",.type = RenderPassType::RayTracing,.fbExtent = WINDOW_EXTENT,
//                        .input = {rtIMG,tlasData,rtUniform,nodeArray,lightData,skyboxTex},
//                        .output = {rtIMG},
//                        .pipeline = {.type = PipelineType::RayTracing,
//                                     .rtShaders ={.chit = "closetHit",.gen = "gen",.miss = "miss",.miss_shadow = "shadowMiss",.ahit ="anyHit"},
//                                     .handleSize = sizeof(RTPC)},
//                        .executeFunc = [=](CommandList& cmd)
//                        {
//                            RTPC rtpc = {rtIMG,tlasData,rtUniform,nodeArray,lightData,skyboxTex};
//                            cmd.PushConstantsForHandles(&rtpc);
//                            cmd.RayTracing();
//                            cmd.TransImage(rg.resourceMap[rtIMG].textureInfo.value(),rg.resourceMap[rtSampleImg].textureInfo.value(),
//                                           VK_IMAGE_LAYOUT_GENERAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//                        }});
//
//    }



    {
        struct alignas(16) PresentPC
        {
            Handle lastAtt;
            Handle visual;
        };

        rg.AddPass({.name = "Present",.type = RenderPassType::Present,.fbExtent = WINDOW_EXTENT,
                        .input = {lighted},
                        .output = {},
                        .pipeline = {.type = PipelineType::RenderQuad, .rsShaders ={"PresentVert", "PresentFrag"},.handleSize = sizeof(PresentPC)},
                        .executeFunc = [=](CommandList& cmd)
                        {
                            PresentPC pushConstants = {lighted};
                            cmd.PushConstantsForHandles(&pushConstants);
                            cmd.DrawRenderQuad();
                        }});
    }
}









