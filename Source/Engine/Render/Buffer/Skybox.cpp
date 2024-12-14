
#include "Skybox.h"
#include "../VulkanContext.h"
Skybox::Skybox(std::string boxPath, std::vector<std::string> paths)
{

    this->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    this->format = VK_FORMAT_R8G8B8A8_SRGB;

    //Allocate buffer
    auto boxModel = ModelLoader::Load(boxPath);
    auto& mesh = boxModel->rootNode->children[0]->meshes[0];
    auto& bufferAlloc=VulkanContext::GetContext().bufferAllocator;

    int verticesSize = sizeof(Vertex)*mesh->verts.size();
    int indicesSize = sizeof(uint32_t)*mesh->index.size();

    this->indicesCount = mesh->index.size();

    VkDeviceAddress vertAddress;
    VkDeviceAddress indexAddress;

    boxVertBuffer = *VulkanContext::GetContext().bufferAllocator.CreateBuffer(verticesSize,
                                                                                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
                                                                                         | VK_BUFFER_USAGE_TRANSFER_DST_BIT
                                                                                         | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                                                         |VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                         VMA_MEMORY_USAGE_GPU_ONLY);

    VkBufferDeviceAddressInfo deviceAddressInfo{};
    deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    deviceAddressInfo.buffer = boxVertBuffer.vk_buffer;

    vertAddress = vkGetBufferDeviceAddress(VulkanContext::GetContext().device,&deviceAddressInfo);

    boxIndexBuffer = *VulkanContext::GetContext().bufferAllocator.CreateBuffer(indicesSize,VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                                                                                                      |VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                                          VMA_MEMORY_USAGE_GPU_ONLY);

    Buffer staging = *VulkanContext::GetContext().bufferAllocator.CreateBuffer(verticesSize+indicesSize,
                                                                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                                               VMA_MEMORY_USAGE_CPU_ONLY);

    void* data =  VulkanContext::GetContext().bufferAllocator.GetMappedMemory(staging);

    memcpy(data,mesh->verts.data(),verticesSize);
    memcpy((char*)data+verticesSize,mesh->index.data(),indicesSize);

    VkBufferCopy vertexCopy{};
    vertexCopy.size = verticesSize;
    vertexCopy.srcOffset = 0;
    vertexCopy.dstOffset = 0;

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    vkCmdCopyBuffer(cmd,staging.vk_buffer,boxVertBuffer.vk_buffer,1,&vertexCopy);

    VkBufferCopy indexCopy{};
    indexCopy.size = indicesSize;
    indexCopy.srcOffset = verticesSize;
    indexCopy.dstOffset = 0;
    vkCmdCopyBuffer(cmd,staging.vk_buffer,boxIndexBuffer.vk_buffer,1,&indexCopy);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);

    //Read Skybox
    auto resTexture = ImageLoader::Load(paths);

    //Create vkImage


    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = VkExtent3D{(uint32_t)resTexture->width,(uint32_t)resTexture->height,1};

    info.mipLevels = 1;
    info.arrayLayers = 6;

    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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

    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    viewInfo.image = this->vk_image;
    viewInfo.format = format;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 6;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if(vkCreateImageView(VulkanContext::GetContext().device,&viewInfo, nullptr,&this->imageView)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create imageview");
    }

    //Copy data to imageView
    Buffer* imgStaging = VulkanContext::GetContext().bufferAllocator.CreateBuffer(resTexture->size,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);

    void* stagingData = VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*imgStaging);

    memcpy(stagingData,resTexture->data.data(),resTexture->size);

    auto cmd2 = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd2, this->vk_image,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,paths.size());

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 6;

    region.imageOffset ={0,0,0};
    region.imageExtent = {(uint32_t)resTexture->width,(uint32_t)resTexture->height,1};


    vkCmdCopyBufferToImage(cmd2, (*imgStaging).vk_buffer, vk_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd2,true);


    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    if(vkCreateSampler(VulkanContext::GetContext().device,&samplerInfo, nullptr, &this->sampler)!=VK_SUCCESS)
    {
        std::runtime_error("Failed to create sampler\n");
    }

    auto cmd4 = VulkanContext::GetContext().BeginSingleTimeCommands();

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd4,vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,1,paths.size());

    VulkanContext::GetContext().EndSingleTimeCommands(cmd4);


}
