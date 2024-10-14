
#include "PBRMaterial.h"
#include "../VulkanContext.h"
PBRMaterial::PBRMaterial()
{
    this->type = MaterialType::Pbr;

}

void PBRMaterial::Setup()
{
    this->type = MaterialType::Pbr;

    for (auto& tex:textures)
    {
        VkDescriptorImageInfo info;
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        info.imageView = tex->allocatedImage.imageView;
        info.sampler = tex->sampler;

        imageInfos.push_back(info);
    }
}

void PBRMaterial::AddTexture(Texture* texture)
{
    texture->TransLayout();
    textures.push_back(texture);
}
