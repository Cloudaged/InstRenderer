
#include "BlockerSearchPass.h"
#include "../VulkanContext.h"
BlockerSearchPass::BlockerSearchPass(GlobalDescriptorData globalData)
: globalData(globalData)
{

}

void BlockerSearchPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    inputAttDes.push_back(attachmentMap["Position"]);
    inputAttDes.push_back(attachmentMap["ShadowMap"]);

    attachmentMap["BlockerSearch"] = AttachmentDes{"BlockerSearch", winWidth, winHeight,
                                             AttachmentUsage::Color,VK_FORMAT_R32G32_SFLOAT, &blockerSearchAtt};

    outputResource.push_back({attachmentMap["BlockerSearch"], AttachmentOP::WriteOnly});
}

void BlockerSearchPass::SetupRenderState()
{
    renderState.layouts[0] = inputAttDesLayout;
    renderState.layouts[1] = globalData.globalDesLayout;
    renderState.CreatePipeline(PipelineType::RenderQuad,
                               passHandle, outputResource.size(),
                               {FILE_PATH("Asset/Shader/slangSPV/BlockerSearchVert.slang.spv"),
                                FILE_PATH("Asset/Shader/slangSPV/BlockerSearchFrag.slang.spv")});
}

void BlockerSearchPass::Execute()
{
    auto presentM = VulkanContext::GetContext().presentManager;

    auto cmd = presentM.presentFrames[presentM.currentFrame].cmd;

    TransAttachmentLayout(cmd);

    VkExtent2D extent = VulkanContext::GetContext().windowExtent;

    std::vector<VkClearValue> clearValues(1);
    clearValues[0].color = {{0.0,0.0,0.0,0.0}};

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

    std::vector<VkDescriptorSet> sets = {inputAttDesSet,globalData.globalDes};

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderState.pipelineLayout,
                            0, sets.size(),sets.data(),0, nullptr);

    vkCmdDraw(cmd, 3, 1, 0, 0);


    vkCmdEndRenderPass(cmd);
    UpdateRecordedLayout();
}
