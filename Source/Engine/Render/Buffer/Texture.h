
#ifndef INSTRENDERER_TEXTURE_H
#define INSTRENDERER_TEXTURE_H
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"
#include "AllocatedImage.h"
#include "../../Resource/ResTexture.h"
class Texture
{
public:
    Texture(AllocatedImage image,TextureType type=TextureType::DontCare);
    TextureType textureType;
    void TransLayout();
    const char *name = nullptr;
    VkDescriptorImageInfo desInfo;
    VkSampler sampler;
    AllocatedImage allocatedImage;
};


#endif //INSTRENDERER_TEXTURE_H
