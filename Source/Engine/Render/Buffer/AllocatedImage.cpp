
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

    if(vmaCreateImage(VulkanContext::GetContext().allocator,&info,&imageAllocateInfo, &this->vk_image,&this->allocation,&this->allocationInfo) != VK_SUCCESS)
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

void AllocatedImage::LoadData(Res::ResTexture* resTexture)
{
    Buffer* staging = VulkanContext::GetContext().bufferAllocator.CreateBuffer(resTexture->size,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);

    void* stagingData = VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*staging);

    /*unsigned char* i = ((unsigned char*)resTexture->data);
    int vecSize = resTexture->size;
    std::vector<unsigned char> vec(i,i+vecSize);
    vec.size();*/
    memcpy(stagingData,resTexture->data.data(),resTexture->size);

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);
    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd, this->vk_image,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset ={0,0,0};
    region.imageExtent = {imageExtent.width,imageExtent.height,1};


    vkCmdCopyBufferToImage(cmd, (*staging).vk_buffer, vk_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd,true);



}
