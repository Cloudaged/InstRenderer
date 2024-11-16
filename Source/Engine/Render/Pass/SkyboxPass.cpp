
#include "SkyboxPass.h"
#include "../VulkanContext.h"

void SkyboxPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    inputResource.push_back(attachmentMap["Lighted"]);

    attachmentMap["Skyboxed"] = AttachmentDes{"Skyboxed",winWidth,winHeight,
                                              AttachmentUsage::ColorAttachment,AttachmentOP::WriteOnly,
                                              VK_FORMAT_R8G8B8A8_SRGB, false, &skyboxAttachment};
    outputAttDes.push_back(attachmentMap["Skyboxed"]);
    outputAttDes.push_back(attachmentMap["Depth"]);
}

void SkyboxPass::Execute()
{
    auto presentM = VulkanContext::GetContext().presentManager;

    auto cmd = presentM.presentFrames[presentM.currentFrame].cmd;

    VkExtent2D extent = VulkanContext::GetContext().windowExtent;


    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = {{0.0,0.0,0.0,0.0}};
    clearValues[1].depthStencil = {1.0f,0};

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = passHandle;
    beginInfo.framebuffer = framebufferHandle;
    beginInfo.renderArea.offset ={0,0};
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount =clearValues.size();
    beginInfo.pClearValues = clearValues.data();
    if(VulkanContext::GetContext().isResize)
        return;

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent.width;
    viewport.height = extent.height;
    //viewport.width =width;
    //viewport.height =height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, renderState.pipeline);

    std::vector<VkDescriptorSet> sets = {globalData.globalDes,inputAttDesSet,imageSet};

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderState.pipelineLayout,
                            0, sets.size(),sets.data(),0, nullptr);

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(cmd,0,1,&globalData.skyboxData.skybox->boxVertBuffer.vk_buffer,offsets);

    vkCmdBindIndexBuffer(cmd,globalData.skyboxData.skybox->boxIndexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd,static_cast<uint32_t>(globalData.skyboxData.skybox->indicesCount),1,0,0,0);

    vkCmdEndRenderPass(cmd);
}

void SkyboxPass::SetupRenderState()
{
    DescriptorBindingSlot slot1 = {0,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};


    CreateDescriptorAndLayout({slot1});

    renderState.layouts[0] = globalData.globalDesLayout;
    renderState.layouts[1] = inputAttDesLayout;
    renderState.layouts[2] = imageLayout;


    renderState.CreatePipeline(PipelineType::Skybox,passHandle,outputAttDes.size()-1,
                               {FILE_PATH("Asset/Shader/spv/Skybox.vert.spv"),
                                FILE_PATH("Asset/Shader/spv/Skybox.frag.spv")});
}

SkyboxPass::SkyboxPass(GlobalDescriptorData data): globalData(data)
{
    this->skybox = data.skyboxData.skybox;
}

void SkyboxPass::CreateDescriptorAndLayout(std::vector<DescriptorBindingSlot> bindings)
{

    std::vector<VkDescriptorSetLayoutBinding> b(bindings.size());

    for (int i = 0; i < b.size(); ++i)
    {
        b[i].binding = bindings[i].bindingPos;
        b[i].descriptorType = bindings[i].type;
        b[i].stageFlags = bindings[i].flags;
        b[i].descriptorCount= 1;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(b.size());
    layoutInfo.pBindings = b.data();

    if (vkCreateDescriptorSetLayout(VulkanContext::GetContext().device, &layoutInfo, nullptr,
                                    &imageLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }


    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &imageLayout;

    vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo,&imageSet);

    std::vector<VkWriteDescriptorSet> writes;
    //Descriptor
    VkDescriptorImageInfo imageInfos;

    imageInfos.imageView = skybox->imageView;
    imageInfos.imageLayout = skybox->layout;
    imageInfos.sampler = skybox->sampler;


    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = imageSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfos;
    descriptorWrite.pTexelBufferView = nullptr;

    writes.push_back(descriptorWrite);

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);
}
