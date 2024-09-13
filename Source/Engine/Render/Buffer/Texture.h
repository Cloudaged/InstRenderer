
#ifndef INSTRENDERER_TEXTURE_H
#define INSTRENDERER_TEXTURE_H
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"
class Texture
{
public:
    const char *name = nullptr;
    VkImage vk_image;
    VkImageView view;
    VkDescriptorImageInfo srv{};
    VkImageLayout imageLayout;
    VkImageUsageFlags usage;
    VkFormat format;
    VkSampler sampler;
    uint32_t width : 16;
    uint32_t height : 16;
    VmaAllocationInfo allocInfo;
    VmaAllocation allocation;
};


#endif //INSTRENDERER_TEXTURE_H
