
#include "CompositionPass.h"
#include "../VulkanContext.h"
#include "../../Common/System/RenderSystem.h"
void CompositionPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    inputAttDes.push_back(attachmentMap["Position"]);
    inputAttDes.push_back(attachmentMap["Normal"]);
    inputAttDes.push_back(attachmentMap["BaseColor"]);
    inputAttDes.push_back(attachmentMap["MetallicRoughness"]);
    inputAttDes.push_back(attachmentMap["ShadowMap"]);

    attachmentMap["Lighted"] = AttachmentDes{"Lighted", winWidth, winHeight,
                                             AttachmentUsage::Color,VK_FORMAT_R8G8B8A8_SRGB};

    outputResource.push_back({attachmentMap["Lighted"], AttachmentOP::WriteOnly});
    outputResource.push_back({attachmentMap["Depth"], AttachmentOP::ReadAndWrite});

}

void CompositionPass::Execute()
{
    auto presentM = VulkanContext::GetContext().presentManager;

    auto cmd = presentM.presentFrames[presentM.currentFrame].cmd;

    TransAttachmentLayout(cmd);


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

    std::vector<VkDescriptorSet> sets = {inputAttDesSet,globData.globalDes};

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderState.pipelineLayout,
                            0, sets.size(),sets.data(),0, nullptr);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxRenderState.pipeline);

    std::vector<VkDescriptorSet> sets2 = {globData.globalDes,skyboxDescriptor};

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            skyboxRenderState.pipelineLayout,
                            0, sets2.size(),sets2.data(),0, nullptr);

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(cmd,0,1,&globData.skyboxData->boxVertBuffer.vk_buffer,offsets);

    vkCmdBindIndexBuffer(cmd,globData.skyboxData->boxIndexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd,static_cast<uint32_t>(globData.skyboxData->indicesCount),1,0,0,0);



    vkCmdEndRenderPass(cmd);
    UpdateRecordedLayout();
}

void CompositionPass::SetupRenderState()
{
    //DescriptorLayout
    renderState.layouts[0] = inputAttDesLayout;
    renderState.layouts[1] = globData.globalDesLayout;
    //Pipeline
    renderState.CreatePipeline(PipelineType::RenderQuad, passHandle, outputResource.size() - 1,
                               {FILE_PATH("Asset/Shader/slangSPV/CompVert.slang.spv"),
                                FILE_PATH("Asset/Shader/slangSPV/CompFrag.slang.spv")});


    CreateSkyboxDes();
    skyboxRenderState.layouts[0] = globData.globalDesLayout;
    skyboxRenderState.layouts[1] = skyboxLayout;
    skyboxRenderState.CreatePipeline(PipelineType::Skybox, passHandle, outputResource.size()-1,
                               {FILE_PATH("Asset/Shader/slangSPV/SkyboxVert.slang.spv"),
                                FILE_PATH("Asset/Shader/slangSPV/SkyboxFrag.slang.spv")});

}

CompositionPass::CompositionPass(GlobalDescriptorData globData): globData(globData)
{

}

void CompositionPass::CreateSkyboxDes()
{
    std::vector<VkDescriptorSetLayoutBinding> b(1);

    b[0].binding = 0;
    b[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    b[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    b[0].descriptorCount= 1;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(b.size());
    layoutInfo.pBindings = b.data();

    if (vkCreateDescriptorSetLayout(VulkanContext::GetContext().device, &layoutInfo, nullptr,
                                    &skyboxLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }


    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &skyboxLayout;

    vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo,&skyboxDescriptor);

    std::vector<VkWriteDescriptorSet> writes;
    //Descriptor
    auto skybox = globData.skyboxData;

    VkDescriptorImageInfo imageInfos;

    imageInfos.imageView = skybox->imageView;
    imageInfos.imageLayout = skybox->layout;
    imageInfos.sampler = skybox->sampler;


    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = skyboxDescriptor;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfos;
    descriptorWrite.pTexelBufferView = nullptr;

    writes.push_back(descriptorWrite);

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);

}





