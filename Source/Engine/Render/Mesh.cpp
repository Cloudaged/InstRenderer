
#include "Mesh.h"
#include "VulkanContext.h"

Mesh::Mesh(std::vector<Vertex> vertData, std::vector<uint32_t> index): vertData(vertData), index(index)
{

    uint32_t verticesSize = vertData.size()*sizeof(Vertex);
    uint32_t indicesSize = index.size()*sizeof(int);

    vertBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(verticesSize,
                                                                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
                                                                      | VK_BUFFER_USAGE_TRANSFER_DST_BIT
                                                                      | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                                      |VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                      VMA_MEMORY_USAGE_GPU_ONLY);

    VkBufferDeviceAddressInfo deviceAddressInfo{};
    deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    deviceAddressInfo.buffer = vertBuffer.vk_buffer;

    vertAddress = vkGetBufferDeviceAddress(VulkanContext::GetContext().device,&deviceAddressInfo);

    indexBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(indicesSize,VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                                                         |VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                             VMA_MEMORY_USAGE_GPU_ONLY);

    Buffer staging = VulkanContext::GetContext().bufferAllocator.CreateBuffer(verticesSize+indicesSize,
                                                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                                              VMA_MEMORY_USAGE_CPU_ONLY);

    void* data =  VulkanContext::GetContext().bufferAllocator.GetMappedMemory(staging);

    memcpy(data,vertData.data(),verticesSize);
    memcpy((char*)data+verticesSize,index.data(),indicesSize);

    VkBufferCopy vertexCopy{};
    vertexCopy.size = verticesSize;
    vertexCopy.srcOffset = 0;
    vertexCopy.dstOffset = 0;

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands();

    vkCmdCopyBuffer(cmd,staging.vk_buffer,vertBuffer.vk_buffer,1,&vertexCopy);

    VkBufferCopy indexCopy{};
    indexCopy.size = indicesSize;
    indexCopy.srcOffset = verticesSize;
    indexCopy.dstOffset = 0;
    vkCmdCopyBuffer(cmd,staging.vk_buffer,indexBuffer.vk_buffer,1,&indexCopy);


    VulkanContext::GetContext().EndSingleTimeCommands(cmd);

    VulkanContext::GetContext().bufferAllocator.DestroyBuffer(staging);
}
