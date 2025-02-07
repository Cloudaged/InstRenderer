
#include "ResourceManager.h"

std::unique_ptr<ResourceManager> ResourceManager::Pinstance = nullptr;

std::string ResourceManager::LoadResource(std::string path)
{
    std::filesystem::path pathObj(path);
    auto extension = pathObj.extension().string();

    if(extension==".fbx"||extension==".gltf")
    {
        auto pModel= ModelLoader::Load(path);
        auto name =GetNameFromPath(path);
        resReg[name]= pModel;
        return name;
    }
    else if(extension==".png"||extension==".jpg")
    {
        /*auto pTexture = ImageLoader::Load(path);
        auto name = GetNameFromPath(path);
        resReg[name] = pTexture;
        return name;*/
    }
    else if(extension==".spv")
    {
        return " ";
    } else
    {
        std::cout<<"can't analysis data\n";
        return " ";
    }

}

ResourceManager &ResourceManager::Get()
{
    if(!Pinstance)
    {
        throw std::runtime_error("the context isn't exist");
    }
    return *(Pinstance.get());}

void ResourceManager::Init()
{
    if(!Pinstance)
    {
        Pinstance.reset(new ResourceManager());
    }
}

ResourceManager::ResourceManager()
{

}

Mesh ResourceManager::TransMesh(std::shared_ptr<Res::ResMesh> resMesh)
{
    return Mesh(resMesh->verts,resMesh->index);
}

Material ResourceManager::TransMaterial(RDG::RenderGraph& renderGraph,std::shared_ptr<Res::ResMaterial> resMaterial)
{
    Material mat;
    for (auto& tex:resMaterial->textures)
    {
        if(tex->textureType==TextureType::BaseColor)
        {
            auto texData = AllocTexture(tex);

            auto handle = renderGraph.AddOuterResource({.name = tex->name,.type = RDG::ResourceType::Texture,
                                                .textureInfo =RDG::TextureInfo{{tex->width,tex->height},
                                                                               TextureUsage::MaterialTexture, VK_FORMAT_R8G8B8A8_SRGB, texData}});
            mat.baseColor = handle;
        } else if(tex->textureType==TextureType::Normal)
        {
            auto texData = AllocTexture(tex);
            auto handle = renderGraph.AddOuterResource({.name = tex->name,.type = RDG::ResourceType::Texture,
                                                          .textureInfo =RDG::TextureInfo{{tex->width, tex->height},
                                                                                         TextureUsage::MaterialTexture, VK_FORMAT_R8G8B8A8_SRGB, texData}});
            mat.normal = handle;
        }else if(tex->textureType==TextureType::RoughnessMetallic)
        {
            auto texData = AllocTexture(tex);
            auto handle = renderGraph.AddOuterResource({.name = tex->name,.type = RDG::ResourceType::Texture,
                                                          .textureInfo =RDG::TextureInfo{{tex->width,tex->height},
                                                                                         TextureUsage::MaterialTexture, VK_FORMAT_R8G8B8A8_SRGB, texData}});
            mat.metallicRoughness = handle;
        }
    }
    return mat;
}

std::shared_ptr<Texture> ResourceManager::AllocTexture(std::shared_ptr<Res::ResTexture> resTexture)
{

    uint32_t mipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(resTexture->width, resTexture->height)))) + 1;

    auto img = std::make_shared<AllocatedImage>(ImageType::Color,VK_FORMAT_R8G8B8A8_SRGB,
                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,
                                                VkExtent2D{(uint32_t)resTexture->width,(uint32_t)resTexture->height},mipLevel,1);

    img->LoadData(resTexture);

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,img->vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,img->mipLevels);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);

    return std::make_shared<Texture>(img,resTexture->textureType);
}

void ResourceManager::CompileModel(GameInstance *instance,std::shared_ptr<Res::ResModel> model)
{
    std::thread t([&]()
    {
        AsynCompile(instance,model);
    });
    t.join();

}

void ResourceManager::AsynCompile(GameInstance *instance, std::shared_ptr<Res::ResModel> model)
{
    std::lock_guard<std::mutex> guard(Locker::Get().loadResourceMtx);

    auto modelRootGo = AddSceneNode(instance,model->rootNode, instance->mainScene->sceneRootGameObject);
    instance->mainScene->sceneRootGameObject->child.insert(modelRootGo);

    instance->mainScene->minPoint = glm::min(model->minPoint,instance->mainScene->minPoint);
    instance->mainScene->maxPoint = glm::max(model->maxPoint,instance->mainScene->maxPoint);
    instance->renderSystem.RecreateRTScene();

    //instance->renderSystem.materialManager.AllocateDescriptorSets();
}

std::shared_ptr<GameObject> ResourceManager::AddSceneNode(GameInstance* instance,std::shared_ptr<Res::ResNode> node,std::shared_ptr<GameObject> parentGO)
{
    //This node
    auto nodeGo =instance->mainScene->CreateObject(node->name,parentGO);
    glm::vec3 pos = {0.0,0.0,0.0};
    glm::vec3 rotation = {0.0,0.0,0.0};
    glm::vec3 scale = {1.0,1.0,1.0};
    auto& transComp = instance->mainScene->reg.get<Transform>(nodeGo->entityID);
    transComp = {pos,rotation,scale};
    //This node's mesh
    for (auto& mesh : node->meshes)
    {
        auto meshGo =instance->mainScene->CreateObject(mesh->name,nodeGo);
        nodeGo->child.insert(meshGo);
        glm::vec3 pos = {0.0,0.0,0.0};
        glm::vec3 rotation = {0.0,0.0,0.0};
        glm::vec3 scale = {1.0,1.0,1.0};
        auto& transComp = instance->mainScene->reg.get<Transform>(meshGo->entityID);
        transComp = {pos,rotation,scale};
        meshGo->componentBits.set((int)ComponentType::Renderable);
        auto meshData= ResourceManager::Get().TransMesh(mesh);
        auto materialData = ResourceManager::Get().TransMaterial(instance->renderSystem.rg, mesh->material);
        auto handle = instance->mainScene->materialAllocator.Allocate();
        instance->mainScene->nodeArr.at(handle) = GeometryNode{meshData.vertAddress,meshData.indexAddress,meshData.transformAddress,0,materialData};
        instance->mainScene->reg.emplace<Renderable>(meshGo->entityID,meshData,materialData,handle);

    }

    //Child
    for (auto& child:node->children)
    {
        auto childGOID = AddSceneNode(instance,child,nodeGo);
        nodeGo->child.insert(childGOID);
    }
    return nodeGo;
}


