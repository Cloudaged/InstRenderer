
#include "PBRMaterial.h"
#include "../VulkanContext.h"
PBRMaterial::PBRMaterial()
{
    this->type = MaterialType::Pbr;

    buffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(int),VK_BUFFER_USAGE_2_UNIFORM_BUFFER_BIT_KHR,VMA_MEMORY_USAGE_CPU_ONLY);
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer.vk_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range =sizeof(int);

    bufferInfos.push_back(bufferInfo);

    for (auto& tex:textures)
    {
        VkDescriptorImageInfo info;
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        info.imageView = tex.view;
        info.sampler = tex.sampler;

        imageInfos.push_back(info);
    }
}
