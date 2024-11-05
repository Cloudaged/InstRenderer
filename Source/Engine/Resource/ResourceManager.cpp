
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
        auto pTexture = ImageLoader::Load(path);
        auto name = GetNameFromPath(path);
        resReg[name] = pTexture;
        return name;
    }
    else if(extension==".spv")
    {

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

Mesh *ResourceManager::TransMesh(Res::ResMesh *resMesh)
{
    Mesh* mesh = new Mesh(resMesh->verts,resMesh->index);
    return mesh;
}

StandardMaterial *ResourceManager::TransMaterial(Res::ResMaterial *resMaterial)
{
    StandardMaterial* mat = new StandardMaterial;
    TextureSlots* slots = new TextureSlots{};
    int a=1;
    for (auto& tex:resMaterial->textures)
    {
        if(tex->textureType==TextureType::BaseColor)
        {
            slots->hasAlbedo=1;
        } else if(tex->textureType==TextureType::Normal)
        {
            slots->hasNormal=1;
        }else if(tex->textureType==TextureType::RoughnessMetallic)
        {
            slots->hasMetallicRoughness=1;
        }

        mat->AddTexture(TransTexture(tex));
    }

    Buffer* buffer=VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(TextureSlots),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    void* mappedMemory= VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*buffer);
    memcpy(mappedMemory,slots,sizeof(TextureSlots));
    mat->slotBuffer = buffer;
    return mat;
}

Texture *ResourceManager::TransTexture(Res::ResTexture* resTexture)
{

    AllocatedImage img(VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,
                       {(uint32_t)resTexture->width,(uint32_t)resTexture->height},1,VK_IMAGE_ASPECT_COLOR_BIT);

    img.LoadData(resTexture);
    auto tex = new Texture(img,resTexture->textureType);
    tex->textureType = resTexture->textureType;
    return tex;
}

void ResourceManager::CompileModel(GameInstance *instance, Res::ResModel *model)
{
    std::thread t([&]()
    {
        AsynCompile(instance,model);
    });
    t.join();
}

void ResourceManager::AsynCompile(GameInstance *instance, Res::ResModel *model)
{
    std::lock_guard<std::mutex> guard(Locker::Get().loadResourceMtx);

    for (auto& mesh:model->meshes)
    {
        //Backend
        auto meshGo =instance->mainScene->CreateObject(mesh->name);
        glm::vec3 pos = {0.0,0.0,0.0};
        glm::vec3 rotation = {0.0,0.0,0.0};
        glm::vec3 scale = {1.0,1.0,1.0};

        instance->mainScene->reg.emplace<Transform>(meshGo->entityID,pos,rotation,scale);
        meshGo->componentBits.set(0);

        auto meshData= ResourceManager::Get().TransMesh(mesh);
        auto materialData = ResourceManager::Get().TransMaterial(mesh->material);
        instance->mainScene->reg.emplace<Renderable>(meshGo->entityID,meshData,materialData);

    }
    instance->renderSystem.materialManager.AllocateDescriptorSets();
}


