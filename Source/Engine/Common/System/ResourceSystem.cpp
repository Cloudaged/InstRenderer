
#include "ResourceSystem.h"
#include "../../Resource/ResourceManager.h"

ResourceSystem::ResourceSystem()
{
}


void ResourceSystem::BeginSystem()
{
    ResourceManager::Init();
}