
#ifndef INSTRENDERER_ALLOCATEDIMAGE_H
#define INSTRENDERER_ALLOCATEDIMAGE_H
#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.h"

#include "../../Resource/ResTexture.h"
class AllocatedImage
{
public:
    AllocatedImage();
    AllocatedImage(VkFormat format,VkImageUsageFlags usageFlags,VkExtent2D extent,int mipLevel,VkImageAspectFlags aspectFlags);
    void LoadData(Res::ResTexture* resTexture);
    VkImage vk_image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
    VkImageUsageFlags usage;
    VmaAllocationInfo allocationInfo;
    VkImageLayout layout;
    int mipLevels=1;
private:
    void GenerateMipmap();
};


#endif //INSTRENDERER_ALLOCATEDIMAGE_H
