
#include "Texture.h"
#include "../VulkanContext.h"


Texture::Texture(std::shared_ptr<AllocatedImage> image,TextureType type): allocatedImage(image)
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
    samplerInfo.maxLod =allocatedImage->mipLevels;

    if(vkCreateSampler(VulkanContext::GetContext().device,&samplerInfo, nullptr, &this->sampler)!=VK_SUCCESS)
    {
        std::runtime_error("Failed to create sampler\n");
    }

}

Texture::~Texture()
{
    if(sampler!=VK_NULL_HANDLE)
    {
        vkDestroySampler(VulkanContext::GetContext().device,sampler, nullptr);
    }
    std::cout<<"TexWork\n";
}

