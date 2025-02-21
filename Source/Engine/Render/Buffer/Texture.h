
#ifndef INSTRENDERER_TEXTURE_H
#define INSTRENDERER_TEXTURE_H
#include "volk.h"
#include "vma/vk_mem_alloc.h"
#include "AllocatedImage.h"
#include "../../Resource/ResTexture.h"
#include "../RenderGraph/TextureUsage.h"
class Texture
{
public:
    Texture(std::shared_ptr<AllocatedImage> image,UsageFlags usage=TextureUsage::Unknown);
    ~Texture();
public:
    VkSampler sampler;
    std::shared_ptr<AllocatedImage> allocatedImage;
};


#endif //INSTRENDERER_TEXTURE_H
