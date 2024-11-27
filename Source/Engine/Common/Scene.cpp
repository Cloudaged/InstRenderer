#include "Scene.h"
#include "iostream"

#include "../Render/VulkanContext.h"
#include "../Resource/ModelLoader.h"

Scene::Scene():
mainCamera(&reg,"mainCamera")
{
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

    bindings.push_back(vp);
    bindings.push_back(light);
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

    //std::vector<VkDescriptorBufferInfo> bufferInfos;
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer =globalData.globBuffer.vk_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(globUniform);

    VkDescriptorBufferInfo lightBufferInfo{};
    lightBufferInfo.buffer =globalData.lightBuffer.vk_buffer;
    lightBufferInfo.offset = 0;
    lightBufferInfo.range = sizeof(LightUniform);


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

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);


}

void Scene::UpdateScene()
{
    globUniform.view = mainCamera.vpMat.view;
    globUniform.proj = mainCamera.vpMat.proj;
    lightUniform.cameraPos= glm::vec4(mainCamera.position,1.0);
    lightUniform.cameraDir = glm::vec4(mainCamera.viewPoint-mainCamera.position,1.0);

    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.globBuffer),&globUniform,sizeof(globUniform));
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.lightBuffer),&lightUniform,sizeof(LightUniform));

}

void Scene::InitSceneData()
{
    mainCamera.InitCamera(glm::vec3{0,0,0},glm::vec3{0,0,1},{0,1,0});

    auto tar = mainCamera.GetCameraTarget();
    globUniform.skyboxProj = glm::perspective(glm::radians(80.0f),
                                              VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height,
                                              0.001f, 256.0f);
    globUniform.skyboxProj[1][1] *=-1;

}

void Scene::InitSkyboxData()
{
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

void Scene::UpdateAspect()
{
    mainCamera.vpMat.proj = glm::perspective(glm::radians(80.0f),
                                        VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height,
                                        5.0f, 20000.0f);
    mainCamera.vpMat.proj[1][1] *=-1;

    globUniform.skyboxProj = glm::perspective(glm::radians(80.0f),
                                              VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height,
                                              0.001f, 256.0f);
    globUniform.skyboxProj[1][1] *=-1;
}

void Scene::InitMainLight()
{
    mainLight = std::static_pointer_cast<Light>(CreateObject("MainLight","Light"));
}

void Scene::UpdateLightData()
{
    int num=0;
    auto view= reg.view<LightComponent,Transform>();
    for (auto& entityID:view)
    {
        auto& lightComp= view.get<LightComponent>(entityID);
        auto& trans = view.get<Transform>(entityID);
        glm::mat4  mat = glm::rotate(glm::mat4(1.0f),trans.rotation.x,{1,0,0});
        mat = glm::rotate(mat,trans.rotation.y,{0,1,0});
        mat = glm::rotate(mat,trans.rotation.z,{0,0,1});
        glm::vec4 rotatedDir = mat*glm::vec4(0.0,0.0,1.0,0.0);
        lightUniform.lights[num] = std::move(LightUnitsInShader{glm::vec4(trans.pos,1.0),rotatedDir,glm::vec4(lightComp.color,1.0),(int)lightComp.type,lightComp.Intensity,lightComp.range});
        num++;
    }
    lightUniform.count=num;
}




