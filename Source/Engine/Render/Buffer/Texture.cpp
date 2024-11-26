
#include "Texture.h"
#include "../VulkanContext.h"


Texture::Texture(AllocatedImage image,TextureType type)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(VulkanContext::GetContext().gpu, &properties);

    //CreateSampler
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor =VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod =allocatedImage.mipLevels;

    if(vkCreateSampler(VulkanContext::GetContext().device,&samplerInfo, nullptr, &this->sampler)!=VK_SUCCESS)
    {
        std::runtime_error("Failed to create sampler\n");
    }

    //Init AllocatedImage
    this->allocatedImage = image;

    //Descriptor img info
    desInfo.sampler = sampler;
    desInfo.imageView = allocatedImage.imageView;
    desInfo.imageLayout = allocatedImage.layout;

}

void Texture::TransLayout()
{
    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,allocatedImage.vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,allocatedImage.mipLevels);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);
}
