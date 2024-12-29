
#ifndef INSTRENDERER_ALLOCATEDIMAGE_H
#define INSTRENDERER_ALLOCATEDIMAGE_H
#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.h"
#include "memory"
#include "../../Resource/ResTexture.h"
enum class ImageType
{
    Color,
    Depth,
    CubeColor
};

class AllocatedImage
{
public:
    AllocatedImage(ImageType type,VkFormat format,VkImageUsageFlags usageFlags,VkExtent2D extent,int mipLevel,int layer);
    void LoadData(std::shared_ptr<Res::ResTexture> resTexture);
    VkImage vk_image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
    VkImageUsageFlags usage;
    VmaAllocationInfo allocationInfo;
    VkImageLayout layout;
    int mipLevels=1;
    int layer = 1;
private:
    void GenerateMipmap();
};


#endif //INSTRENDERER_ALLOCATEDIMAGE_H
