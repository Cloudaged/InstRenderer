
#include "AllocatedImage.h"

#include "../VulkanContext.h"

AllocatedImage::AllocatedImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent, int mipLevel,
                               VkImageAspectFlags aspectFlags)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = VkExtent3D{extent.width,extent.height,1};

    info.mipLevels = mipLevel;
    info.arrayLayers = 1;

    info.samples = VK_SAMPLE_COUNT_1_BIT;

    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo imageAllocateInfo{};
    imageAllocateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if(vmaCreateImage(VulkanContext::GetContext().allocator,&info,&imageAllocateInfo, &this->vk_image,&this->allocation,&this->info)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create allocatedImage");
    }

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = nullptr;

    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.image = this->vk_image;
    viewInfo.format = format;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevel;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.aspectMask = aspectFlags;

    if(vkCreateImageView(VulkanContext::GetContext().device,&viewInfo, nullptr,&this->imageView)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create imageview");
    }

    this->imageFormat = format;
    this->imageExtent = VkExtent3D{extent.width,extent.height,1};
    this->usage = usageFlags;
}

AllocatedImage::AllocatedImage()
{

}
