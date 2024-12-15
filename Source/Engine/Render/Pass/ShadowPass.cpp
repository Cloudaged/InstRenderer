
#include "ShadowPass.h"
#include "../VulkanContext.h"

ShadowPass::ShadowPass(GlobalDescriptorData globalDescriptorData): globalDescriptorData(globalDescriptorData)
{
}

void ShadowPass::SetupAttachments()
{
    shadowMapWidth = 2048;
    shadowMapHeight = 2048;
    attachmentMap["ShadowMap"] = AttachmentDes{"ShadowMap", (int)shadowMapWidth, (int)shadowMapHeight,
                                               AttachmentUsage::ShadowMap,VK_FORMAT_D32_SFLOAT, &shadowMapAttachment};

    outputResource.push_back({attachmentMap["ShadowMap"], AttachmentOP::WriteOnly});
}

void ShadowPass::SetupRenderState()
{
    renderState.layouts[0] = globalDescriptorData.globalDesLayout;

    renderState.CreatePipeline(PipelineType::Mesh,
                               passHandle, outputResource.size() - 1,
                               {FILE_PATH("Asset/Shader/slangSPV/ShadowVert.slang.spv"),
                                FILE_PATH("Asset/Shader/slangSPV/ShadowFrag.slang.spv")});
}

void ShadowPass::Execute(entt::view<entt::get_t<Renderable, Transform>> compView)
{
    std::lock_guard<std::mutex> guard(Locker::Get().loadResourceMtx);

    auto presentM = VulkanContext::GetContext().presentManager;

    auto cmd = presentM.presentFrames[presentM.currentFrame].cmd;

    TransAttachmentLayout(cmd);

    VkExtent2D extent = {shadowMapWidth,shadowMapHeight};

    std::vector<VkClearValue> clearValues(outputResource.size());
    clearValues[0].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = passHandle;
    beginInfo.framebuffer = this->framebufferHandle;
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

    //BeginRender pass
    for (const auto& entity:compView)
    {
        auto renderComponents = compView.get<Renderable>(entity);
        auto transComponents = compView.get<Transform>(entity);
        modelMatrix = {transComponents.globalTransform};

        std::vector<VkDescriptorSet> sets = {globalDescriptorData.globalDes};
        //Bind
        vkCmdPushConstants(cmd,renderState.pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(glm::mat4),&modelMatrix);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                renderState.pipelineLayout,
                                0, sets.size(),sets.data(),0, nullptr);

        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(cmd,0,1,&renderComponents.mesh->vertBuffer.vk_buffer, offsets);

        vkCmdBindIndexBuffer(cmd,renderComponents.mesh->indexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
        //Draw
        vkCmdDrawIndexed(cmd,static_cast<uint32_t>(renderComponents.mesh->index.size()),1,0,0,0);
    }

    vkCmdEndRenderPass(cmd);
    UpdateRecordedLayout();
}


