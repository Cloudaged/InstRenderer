
#include "BufferAllocator.h"
#include "../VulkanContext.h"

#define VMA_IMPLEMENTATION
#include "VMA/vk_mem_alloc.h"

Buffer* BufferAllocator::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
    Buffer* buffer = new Buffer;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = allocSize;
    bufferInfo.usage = usage;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = memoryUsage;
    vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;


    if(vmaCreateBuffer(VulkanContext::GetContext().allocator,&bufferInfo,&vmaallocInfo,&buffer->vk_buffer,&buffer->allocation,&buffer->info)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create allocatedBuffer!");
    }
    return buffer;
}

AllocatedImage* BufferAllocator::CreateImageBuffer(VkExtent2D extent2D, VkFormat format, VkImageUsageFlags usage)
{
    AllocatedImage* allocatedImage  = new AllocatedImage();
    allocatedImage->imageFormat = format;
    allocatedImage->imageExtent.width = extent2D.width;
    allocatedImage->imageExtent.height = extent2D.height;
    allocatedImage->usage = usage;

    VmaAllocationCreateInfo imageAllocateInfo{};
    imageAllocateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = {extent2D.width,extent2D.height,1};
    info.mipLevels = 0;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    auto result = vmaCreateImage(VulkanContext::GetContext().allocator,&info,&imageAllocateInfo,&allocatedImage->vk_image,
                                 &allocatedImage->allocation, &allocatedImage->allocationInfo);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create allocatedImage");
    }

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = nullptr;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.image = allocatedImage->vk_image;
    viewInfo.format = format;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 0;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if(vkCreateImageView(VulkanContext::GetContext().device,&viewInfo, nullptr,&allocatedImage->imageView)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create imageview");
    }

    return allocatedImage;
}

void *BufferAllocator::GetMappedMemory(Buffer allocatedBuffer)
{

    return allocatedBuffer.allocation->GetMappedData();
}

void BufferAllocator::DestroyBuffer(Buffer buffer)
{
    vmaDestroyBuffer(VulkanContext::GetContext().allocator,buffer.vk_buffer,buffer.allocation);
}

void
BufferAllocator::TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout,int mipLevels,int layerCount)
{
    VkImageMemoryBarrier2 imageBarrier;
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageBarrier.pNext = nullptr;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageBarrier.oldLayout = srcLayout;
    imageBarrier.newLayout = dstLayout;

    bool isDepth  = dstLayout==VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL||dstLayout==VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkImageAspectFlags aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;


    VkImageSubresourceRange subImage {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = mipLevels;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = layerCount;


    imageBarrier.subresourceRange = subImage;
    imageBarrier.image = image;

    VkDependencyInfo depInfo {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);


}
