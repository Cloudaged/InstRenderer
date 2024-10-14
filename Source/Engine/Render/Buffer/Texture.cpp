
#include "Texture.h"
#include "../VulkanContext.h"


Texture::Texture(AllocatedImage image)
{

    //CreateSampler
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

    //Init AllocatedImage
    this->allocatedImage = image;

    //Descriptor img info
    desInfo.sampler = sampler;
    desInfo.imageView = allocatedImage.imageView;
    desInfo.imageLayout = allocatedImage.layout;

}

void Texture::TransLayout()
{
    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands();

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,allocatedImage.vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd);
}
