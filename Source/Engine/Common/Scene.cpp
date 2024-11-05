
#include "Scene.h"
#include "iostream"

#include "../Render/VulkanContext.h"


Scene::Scene():mainCamera(&reg,"mainCamera")
{
}

Scene::~Scene()
{

}

GameObject* Scene::CreateObject(std::string name)
{
    /*auto type_hash = entt::hashed_string::value(type.c_str());
    auto name_hash = entt::hashed_string::value(name.c_str());

    auto type_meta = entt::resolve(type_hash);

    auto instance = type_meta.construct(&reg,name);*/

    GameObject* go= new GameObject(&reg,name);

    objects.push_back(go);

    return go;
}

GameObject* Scene::CreateObject(std::string name, int parent)
{
   /* using namespace entt::literals;
    auto type_hash = entt::hashed_string::value(type.c_str());
    auto name_hash = entt::hashed_string::value(name.c_str());

    auto type_meta = entt::resolve(type_hash);
    auto parent_meta = entt::resolve(type_hash).data("parent"_hs);


    auto instance = type_meta.construct(&reg,name);

    parent_meta.set(instance,parent);*/

    GameObject* go= new GameObject(&reg,name);
    go->parent = parent;

    objects.push_back(go);

    return go;
}


void Scene::DeleteObject(int id)
{
    //Delete Child
    for (int i = 0; i < objects.size(); ++i)
    {
        GameObject* go = objects[i];
        if (go->parent==id)
        {
            Destroy(i);
        }
    }
}

void Scene::Destroy(int i)
{
    GameObject* go =  objects[i];
    reg.destroy(go->entityID);
    objects.erase(objects.cbegin()+i);
    delete go;
}

void Scene::RenameObject(int id, std::string dstName)
{
    for (int i = 0; i < objects.size(); ++i)
    {
        GameObject* go =  (GameObject*)objects[i];
        if(static_cast<int>(go->entityID)==id)
        {
            go->name = dstName;
        }

    }
}

GameObject *Scene::GetGameObject(int id)
{
    for (auto& meta:objects)
    {
        GameObject* go = (GameObject*)meta;
        if(static_cast<int>(go->entityID)==id)
        {
            return go;
        }
    }
    std::cout<<"Can't find game object"<<id<<"\n";
}

void Scene::InitGlobalSet()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding vp;
    vp.binding=0;
    vp.descriptorCount=1;
    vp.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings.push_back(vp);
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
    globalData.buffer= *VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(globUniform),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer =globalData.buffer.vk_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(globUniform);

    std::vector<VkWriteDescriptorSet> writes;
    VkWriteDescriptorSet descriptorWrites{};
    descriptorWrites.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites.dstSet =globalData.globalDes;
    descriptorWrites.dstBinding = 0;
    descriptorWrites.dstArrayElement = 0;
    descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites.descriptorCount = 1;
    descriptorWrites.pBufferInfo = &bufferInfo;
    writes.push_back(descriptorWrites);

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);


}

void Scene::UpdateScene()
{
    globUniform.view = mainCamera.vpMat.view;
    globUniform.proj = mainCamera.vpMat.proj;

    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.buffer),&globUniform,sizeof(globUniform));
}

void Scene::InitSceneData()
{
    mainCamera.InitCamera(glm::vec3{0,0,30},glm::vec3{0,0,0},{0,1,0});
}

template<typename T>
void Scene::AddComponent(int objID)
{
    GameObject* go;
    for (int i = 0; i < objects.size(); ++i)
    {
        GameObject* tempGo =  objects[i];
        if(static_cast<int>(go->entityID)==objID)
        {
            go = tempGo;
            break;
        }
    }
    if(go)
    {
        reg.emplace<T>(go->entityID);
    }

}



