
#ifndef INSTRENDERER_TEXTURE_H
#define INSTRENDERER_TEXTURE_H
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"
#include "AllocatedImage.h"
#include "../../Resource/ResTexture.h"
class Texture
{
public:
    Texture(std::shared_ptr<AllocatedImage> image,TextureType type=TextureType::DontCare);
    ~Texture();
public:
    VkSampler sampler;
    std::shared_ptr<AllocatedImage> allocatedImage;
};


#endif //INSTRENDERER_TEXTURE_H
