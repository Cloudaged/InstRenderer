
#include "MaterialManager.h"
#include "VulkanContext.h"
void MaterialManager::Setup(VkDescriptorSetLayout* layout,entt::view<entt::get_t<Renderable>> view)
{
    this->materialLayout = layout;
    this->view = view;
    AllocateDescriptorSets();
    CreateNullImg();
}

void MaterialManager::AllocateDescriptorSets()
{
    for(auto& entity:view)
    {
        auto& component = view.get<Renderable>(entity);
        if (component.material->isSetup)
        {
            continue;
        }
        switch (component.material->type)
        {
            case MaterialType::Pbr:
            {
                AllocatePBRDes(component.material);
                component.material->isSetup= true;
                break;
            }
            default:break;
        }
    }
}

void MaterialManager::AllocatePBRDes(StandardMaterial* material)
{
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = materialLayout;

    if (vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo, &material->set) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate ds");
    }

    //Get write
    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorImageInfo> imgInfos(3);
    VkDescriptorBufferInfo bufferInfo;
    //Detect img
    {
        bufferInfo.buffer = material->slotBuffer->vk_buffer;
        bufferInfo.range = sizeof(TextureSlots);
        bufferInfo.offset = 0;

        VkWriteDescriptorSet descriptorWrites{};
        descriptorWrites.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.dstSet = material->set;
        descriptorWrites.dstBinding = 0;
        descriptorWrites.dstArrayElement = 0;
        descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites.descriptorCount = 1;
        descriptorWrites.pBufferInfo = &bufferInfo;
        writes.push_back(descriptorWrites);
    }

    for (int i = 0; i < 3; ++i)
    {
        if(i>=material->textures.size())
        {
            imgInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgInfos[i].imageView = nullTexture->allocatedImage.imageView;
            imgInfos[i].sampler = nullTexture->sampler;
        } else
        {
            imgInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgInfos[i].imageView = material->textures[i]->allocatedImage.imageView;
            imgInfos[i].sampler = material->textures[i]->sampler;
        }


        VkWriteDescriptorSet descriptorWrites{};
        descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.dstSet = material->set;
        descriptorWrites.dstBinding = i+1;

        descriptorWrites.dstArrayElement = 0;
        descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites.descriptorCount = 1;
        descriptorWrites.pImageInfo = &imgInfos[i];
        writes.push_back(descriptorWrites);
    }

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);
    //memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(material.buffer),&a,sizeof(int));
}

void MaterialManager::CreateNullImg()
{
    std::vector<unsigned char> data = {0};
    Res::ResTexture resTexture(1,1,4,data,"");
    resTexture.textureType = TextureType::DontCare;
    AllocatedImage img(VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,
                       {1,1},1,VK_IMAGE_ASPECT_COLOR_BIT);
    img.LoadData(&resTexture);

    auto cmd = VulkanContext::GetContext().BeginSingleTimeCommands(true);

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,img.vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VulkanContext::GetContext().EndSingleTimeCommands(cmd, true);
    nullTexture = new Texture(img,resTexture.textureType);
    nullTexture->textureType = resTexture.textureType;

}



