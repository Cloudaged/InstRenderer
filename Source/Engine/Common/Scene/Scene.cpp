#include "Scene.h"
#include "iostream"

#include "../../Render/VulkanContext.h"
#include "../../Resource/ModelLoader.h"
RenderSettingData globalRenderSettingData{};
Scene::Scene():
mainCamera(&reg,"mainCamera")
{
    InitGlobalSet();
    InitSkyboxData();
    InitMainCamera();
    InitMainLight();
}

Scene::~Scene()
{

}

std::shared_ptr<GameObject> Scene::CreateObject(std::string name,std::string type)
{
    std::shared_ptr<GameObject> go;
    if(type=="GameObject")
    {
        go = std::make_shared<GameObject>(&reg,name,type);
    }
    if(type=="Light")
    {
        auto lightGo = std::make_shared<Light>(&reg,name,type);
        lights.push_back(lightGo);
        go = std::static_pointer_cast<GameObject>(lightGo);
        UpdateLightData();
    }
    objects.push_back(go);

    return go;
}

std::shared_ptr<GameObject> Scene::CreateObject(std::string name, int parent,std::string type)
{

    std::shared_ptr<GameObject> go= std::make_shared<GameObject>(&reg,name,type);
    go->parent = parent;

    objects.push_back(go);

    return go;
}


void Scene::DeleteObject(int id)
{
    //Delete Child
    for (int i = 0; i < objects.size(); ++i)
    {
        std::shared_ptr<GameObject> go = objects[i];
        if (go->parent==id)
        {
            Destroy(i);
        }
    }
}

void Scene::Destroy(int i)
{
    std::shared_ptr<GameObject> go =  objects[i];
    reg.destroy(go->entityID);
    objects.erase(objects.cbegin()+i);
    go.reset();
}

void Scene::RenameObject(int id, std::string dstName)
{
    for (int i = 0; i < objects.size(); ++i)
    {
        std::shared_ptr<GameObject> go =  objects[i];
        if(static_cast<int>(go->entityID)==id)
        {
            go->name = dstName;
        }

    }
}

std::shared_ptr<GameObject> Scene::GetGameObject(int id)
{
    for (auto& obj:objects)
    {
        std::shared_ptr<GameObject> go = obj;
        if(static_cast<int>(go->entityID)==id)
        {
            return go;
        }
    }
    std::cout<<"Can't find game object"<<id<<"\n";
    return nullptr;
}

void Scene::InitGlobalSet()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding vp;
    vp.binding=0;
    vp.descriptorCount=1;
    vp.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorSetLayoutBinding light;
    light.binding=1;
    light.descriptorCount=1;
    light.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    light.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

   /* VkDescriptorSetLayoutBinding graphicSettingBinding;
    graphicSettingBinding.binding=2;
    graphicSettingBinding.descriptorCount=1;
    graphicSettingBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    graphicSettingBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;*/

    bindings.push_back(vp);
    bindings.push_back(light);
    //bindings.push_back(graphicSettingBinding);
    //Layout

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(VulkanContext::GetContext().device, &layoutInfo, nullptr,
                                    &globalData.globalDesLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }

    //Allocate Descriptor
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &globalData.globalDesLayout;

    if (vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo,&globalData.globalDes) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate ds");
    }
    //Allocate
    int a = sizeof(lightUniform);
    globalData.globBuffer= *VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(globUniform),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    globalData.lightBuffer =*VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(LightUniform),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    globalData.graphicBuffer = *VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(RenderSettingData),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);

    //std::vector<VkDescriptorBufferInfo> bufferInfos;
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer =globalData.globBuffer.vk_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(globUniform);

    VkDescriptorBufferInfo lightBufferInfo{};
    lightBufferInfo.buffer =globalData.lightBuffer.vk_buffer;
    lightBufferInfo.offset = 0;
    lightBufferInfo.range = sizeof(LightUniform);

    /*VkDescriptorBufferInfo graphicSettingsInfo{};
    graphicSettingsInfo.buffer =globalData.graphicBuffer.vk_buffer;
    graphicSettingsInfo.offset = 0;
    graphicSettingsInfo.range = sizeof(RenderSettingData);*/

    std::vector<VkWriteDescriptorSet> writes;
    VkWriteDescriptorSet descriptorWrites1{};
    descriptorWrites1.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites1.dstSet =globalData.globalDes;
    descriptorWrites1.dstBinding = 0;
    descriptorWrites1.dstArrayElement = 0;
    descriptorWrites1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites1.descriptorCount = 1;
    descriptorWrites1.pBufferInfo = &bufferInfo;
    writes.push_back(descriptorWrites1);


    VkWriteDescriptorSet descriptorWrites2{};
    descriptorWrites2.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites2.dstSet =globalData.globalDes;
    descriptorWrites2.dstBinding = 1;
    descriptorWrites2.dstArrayElement = 0;
    descriptorWrites2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites2.descriptorCount = 1;
    descriptorWrites2.pBufferInfo = &lightBufferInfo;
    writes.push_back(descriptorWrites2);

    /*VkWriteDescriptorSet descriptorWrites3{};
    descriptorWrites3.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites3.dstSet =globalData.globalDes;
    descriptorWrites3.dstBinding = 2;
    descriptorWrites3.dstArrayElement = 0;
    descriptorWrites3.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites3.descriptorCount = 1;
    descriptorWrites3.pBufferInfo = &graphicSettingsInfo;
    writes.push_back(descriptorWrites3);*/

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);


}

void Scene::UpdateScene()
{

    mainCamera.cameraData.aspect = (float)VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height;

    globUniform.skyboxProj = glm::perspective(glm::radians(80.0f),
                                              mainCamera.cameraData.aspect,
                                              0.001f, 256.0f);
    globUniform.skyboxProj[1][1] *=-1;

    mainCamera.vpMat.proj = glm::perspective(mainCamera.cameraData.fov, mainCamera.cameraData.aspect,
                                             mainCamera.cameraData.nearPlane, mainCamera.cameraData.farPlane);
    mainCamera.vpMat.proj[1][1] *=-1;

    globUniform.proj = mainCamera.vpMat.proj;
    auto [lightV,lightP] = mainLight->GetLightMatrix(&reg,minPoint,maxPoint);
    globUniform.lightViewMat = lightV;
    globUniform.lightProjMat = lightP;
    lightUniform.cameraPos= glm::vec4(mainCamera.position,1.0);
    lightUniform.cameraDir = glm::vec4(mainCamera.viewPoint-mainCamera.position,1.0);

    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.globBuffer),&globUniform,sizeof(globUniform));
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.lightBuffer),&lightUniform,sizeof(LightUniform));
    //memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.graphicBuffer),&globalRenderSettingData,sizeof(globalRenderSettingData));

  }


void Scene::InitSkyboxData()
{
    globUniform.skyboxProj = glm::perspective(glm::radians(80.0f),
                                              VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height,
                                              0.001f, 256.0f);
    globUniform.skyboxProj[1][1] *=-1;

    std::string boxPath = FILE_PATH("Asset/Skybox/Box/Box.gltf");

    std::string texturePath = FILE_PATH("Asset/Skybox/Textures/");

    std::vector<std::string> texPaths = {texturePath+"posx.jpg",
                                         texturePath+"negx.jpg",
                                         texturePath+"posy.jpg",
                                         texturePath+"negy.jpg",
                                         texturePath+"posz.jpg",
                                         texturePath+"negz.jpg"};

    globalData.skyboxData.skybox = new Skybox(boxPath,texPaths);
}


void Scene::InitMainLight()
{

    mainLight = std::static_pointer_cast<Light>(CreateObject("MainLight","Light"));
    auto& transComp = reg.get<Transform>(mainLight->entityID);
    transComp.rotation = {90,0,0};
    UpdateLightData();

}

void Scene::UpdateLightData()
{
    int num=0;
    auto view= reg.view<LightComponent,Transform>();
    for (auto& entityID:view)
    {
        auto& lightComp= view.get<LightComponent>(entityID);
        auto& trans = view.get<Transform>(entityID);
        glm::mat4 mat = EngineMath::GetRotateMatrix(trans.rotation);
        glm::vec4 rotatedDir = mat*glm::vec4(0.0,0.0,1.0,0.0);
        lightUniform.lights[num] = std::move(LightUnitsInShader{glm::vec4(trans.pos,1.0),rotatedDir,
                                                                glm::vec4(lightComp.color,1.0),
                                                                (int)lightComp.type,lightComp.Intensity,
                                                                glm::radians(lightComp.range),lightComp.lightSize,lightComp.attenuation});
        num++;
    }
    lightUniform.count=num;
}


void Scene::onCameraUpdate(Camera &camera)
{

    camera.vpMat.view = glm::lookAt(camera.position,
                                    camera.viewPoint+camera.position,camera.yAxis);

    globUniform.view = camera.vpMat.view;
    lightUniform.cameraPos= glm::vec4(camera.position,1.0);
    lightUniform.cameraDir = glm::vec4(camera.viewPoint-camera.position,1.0);


}

void Scene::onLightUpdate(Light* light)
{

}

void Scene::InitMainCamera()
{
    mainCamera.InitCamera(glm::vec3{0,0,0},glm::vec3{0,0,1},{0,1,0});
    this->globUniform.view = mainCamera.vpMat.view;
    this->globUniform.proj = mainCamera.vpMat.proj;
    mainCamera.AddObserver(this);
}



