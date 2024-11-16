
#include "CompositionPass.h"
#include "../VulkanContext.h"

void CompositionPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    inputResource.push_back(attachmentMap["BaseColor"]);
    inputResource.push_back(attachmentMap["Normal"]);
    inputResource.push_back(attachmentMap["Position"]);


    attachmentMap["Lighted"] = AttachmentDes{"Lighted",winWidth,winHeight,
                                             AttachmentUsage::ColorAttachment,AttachmentOP::WriteOnly,
                                             VK_FORMAT_R8G8B8A8_SRGB, false, &lightAttachment};

    outputAttDes.push_back(attachmentMap["Lighted"]);
    outputAttDes.push_back(attachmentMap["Depth"]);

}

void CompositionPass::Execute()
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

    std::vector<VkDescriptorSet> sets = {inputAttDesSet};

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderState.pipelineLayout,
                            0, sets.size(),sets.data(),0, nullptr);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd);
}

void CompositionPass::SetupRenderState()
{
    //DescriptorLayout
    renderState.layouts[0] = inputAttDesLayout;
    //Pipeline
    renderState.CreatePipeline(PipelineType::RenderQuad,passHandle,outputAttDes.size()-1,
                               {FILE_PATH("Asset/Shader/spv/Comp.vert.spv"),
                                FILE_PATH("Asset/Shader/spv/Comp.frag.spv")});
}

CompositionPass::CompositionPass()
{

}





