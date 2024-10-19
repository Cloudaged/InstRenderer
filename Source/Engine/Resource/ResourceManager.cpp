
#include "ResourceManager.h"

std::unique_ptr<ResourceManager> ResourceManager::Pinstance = nullptr;

void ResourceManager::LoadResource(std::string path)
{
    std::filesystem::path pathObj(path);
    auto extension = pathObj.extension().string();

    if(extension==".fbx")
    {
       auto pModel= ModelLoader::Load(path);
       resReg[GetNameFromPath(path)]= pModel;
    }
    else if(extension==".png")
    {

    }
    else if(extension==".spv")
    {

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


