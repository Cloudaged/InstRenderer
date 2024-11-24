
#include "AllocatedImage.h"

#include "../VulkanContext.h"

AllocatedImage::AllocatedImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent, int mipLevel,
                               VkImageAspectFlags aspectFlags):mipLevels(mipLevel)
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
    memcpy(stagingData,resTexture->data.data(),resTexture->size);

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);
    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd, this->vk_image,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,mipLevels);

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

    if(mipLevels!=1)
    {
        GenerateMipmap();
    }


}

void AllocatedImage::GenerateMipmap()
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(VulkanContext::GetContext().gpu, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = this->vk_image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer=0;
    barrier.subresourceRange.layerCount=1;
    barrier.subresourceRange.levelCount = 1;

    int32_t  mipWidth = this->imageExtent.width;
    int32_t mipHeight = this->imageExtent.height;

    for (int i = 1; i < this->mipLevels; ++i)
    {
        barrier.subresourceRange.baseMipLevel = i-1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(cmd,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,0,
                             0, nullptr,
                             0, nullptr,
                             1,&barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0,0,0};
        blit.srcOffsets[1] = {mipWidth,mipHeight,1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i-1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount=1;
        blit.dstOffsets[0] = {0,0,0};
        blit.dstOffsets[1] = {mipWidth>1?mipWidth/2:1,mipHeight>1?mipHeight/2:1,1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel=i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount=1;

        vkCmdBlitImage(cmd,
                       vk_image,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,&blit,
                       VK_FILTER_LINEAR);
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);

}
