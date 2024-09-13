
#include "MaterialManager.h"
#include "VulkanContext.h"
void MaterialManager::Setup(VkDescriptorSetLayout* layout,entt::view<entt::get_t<Renderable>> view)
{
    this->materialLayout = layout;
    this->view = view;
    AllocateDescriptorSets();

}

void MaterialManager::AllocateDescriptorSets()
{
    for(auto& entity:view)
    {
        auto& component = view.get<Renderable>(entity);
        switch (component.material.type)
        {
            case MaterialType::Pbr:
                AllocatePBRDes(component.material);
        }
    }
}

void MaterialManager::AllocatePBRDes(PBRMaterial& material)
{
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = materialLayout;

    if (vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo, &material.set) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate ds");
    }

    //Get write
    std::vector<VkWriteDescriptorSet> writes;
    int index=0;
    for (auto& bufferInfo:material.bufferInfos)
    {
        VkWriteDescriptorSet descriptorWrites{};
        descriptorWrites.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.dstSet = material.set;
        descriptorWrites.dstBinding = index;
        descriptorWrites.dstArrayElement = 0;
        descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites.descriptorCount = 1;
        descriptorWrites.pBufferInfo = &bufferInfo;
        index++;
    }

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);
    int a =3;
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(material.buffer),&a,sizeof(int));
}



