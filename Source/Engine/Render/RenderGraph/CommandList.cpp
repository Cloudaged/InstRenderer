
#include "CommandList.h"
#include "../VulkanContext.h"
namespace RDG
{
    void CommandList::BeginCommand()
    {
        auto& pr =VulkanContext::GetContext().presentManager;
        this->cmd = pr.presentFrames[pr.currentFrame].cmd;
        pr.BeginRecordCommand();
    }

    void CommandList::EndCommand()
    {
        vkEndCommandBuffer(cmd);
    }

    bool CommandList::BeginRenderPass(const PassRef& passRef)
    {
        curPass = std::make_shared<PassRef>(passRef);
        if(passRef.type==RenderPassType::RayTracing)
        {
            return true;
        }else if(passRef.type==RenderPassType::Compute)
        {
            return true;
        }
        auto presentM = VulkanContext::GetContext().presentManager;

        VkExtent2D extent = passRef.fbExtent.GetVkExtent();
        VkRenderPassBeginInfo beginInfo{};

        if(passRef.type==RenderPassType::Present)
        {
            VkClearValue clearValue{};
            clearValue.color = {0,0,0,0};
            clearValue.depthStencil = {0,0};
            beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            beginInfo.renderPass = passRef.data.passHandle;
            beginInfo.framebuffer = presentM.presentFrames[presentM.currentFrame].framebuffer;
            beginInfo.renderArea.offset = {0, 0};
            beginInfo.renderArea.extent = extent;
            beginInfo.clearValueCount = 1;
            beginInfo.pClearValues = &clearValue;

            vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        } else
        {
            std::vector<VkClearValue> clearValues(passRef.output.size());
            if (!passRef.hasDepth)
            {
                for (int i = 0; i < passRef.output.size(); ++i)
                {
                    clearValues[i].color={0.0, 0, 0, 0};
                    //clearValues[i].depthStencil = {0,0};
                }
            } else
            {
                for (int i = 0; i < passRef.output.size() - 1; ++i)
                {
                    clearValues[i].color = {0.0, 0, 0, 0};
                    //clearValues[i].depthStencil = {0,0};
                }
                //clearValues[passRef.output.size()-1].color ={0.0f, 0,0,0};
                clearValues[passRef.output.size()-1].depthStencil ={1.0f, 0};
            }

            beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            beginInfo.renderPass = passRef.data.passHandle;
            beginInfo.framebuffer = passRef.data.framebufferHandle;
            beginInfo.renderArea.offset = {0, 0};
            beginInfo.renderArea.extent = extent;
            beginInfo.clearValueCount = clearValues.size();
            beginInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        }



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
        return true;
    }

    void CommandList::EndRenderPass()
    {
        if(curPass->type==RenderPassType::RayTracing)
        {
            return;
        }else if(curPass->type==RenderPassType::Compute)
        {
            return;
        }
        vkCmdEndRenderPass(cmd);
    }

    void CommandList::PushConstantsForHandles(void* data)
    {
        vkCmdPushConstants(cmd,curPass->pipeline.pipelineLayout,VK_SHADER_STAGE_ALL,0,curPass->pipeline.handleSize,data);
    }

    void CommandList::BindDescriptor()
    {
        VkPipelineBindPoint bindingPoint;
        switch (curPass->type)
        {
            case RenderPassType::Raster: bindingPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;break;
            case RenderPassType::RayTracing: bindingPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;break;
            case RenderPassType::Compute: bindingPoint = VK_PIPELINE_BIND_POINT_COMPUTE;break;
            default:bindingPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;break;
        }
        vkCmdBindDescriptorSets(cmd, bindingPoint,
                                curPass->pipeline.pipelineLayout,
                                0, 1,&VulkanContext::GetContext().bindlessSet,0, nullptr);
    }

    void CommandList::BindPipeline()
    {
        VkPipelineBindPoint bindingPoint;
        switch (curPass->type)
        {
            case RenderPassType::Raster: bindingPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;break;
            case RenderPassType::RayTracing: bindingPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;break;
            case RenderPassType::Compute: bindingPoint = VK_PIPELINE_BIND_POINT_COMPUTE;break;
            default:bindingPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;break;
        }
        vkCmdBindPipeline(cmd, bindingPoint, curPass->pipeline.pipeline);
    }

    void CommandList::DrawMesh(const Mesh& mesh)
    {
        if(mesh.vertBuffer.vk_buffer==VK_NULL_HANDLE||mesh.indexBuffer.vk_buffer==VK_NULL_HANDLE)
        {
            return;
        }
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd,0,1,&mesh.vertBuffer.vk_buffer, offsets);

        vkCmdBindIndexBuffer(cmd,mesh.indexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
        //Draw
        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(mesh.indexCount), 1, 0, 0, 0);
    }

    void CommandList::DrawRenderQuad()
    {
        vkCmdDraw(cmd, 3, 1, 0, 0);
    }

    void CommandList::RayTracing()
    {
        auto& sbt = curPass->pipeline.sbt;
        auto extension = curPass->fbExtent.GetVkExtent();
        VkStridedDeviceAddressRegionKHR callableRegion = {};
        vkCmdTraceRaysKHR(cmd, &sbt.genRegion, &sbt.missRegion, &sbt.hitRegion, &callableRegion,
                          extension.width, extension.height, 1);
    }


} // RDG
void RDG::CommandList::TransImage(TextureInfo& src,TextureInfo& dst,VkImageLayout srcFinalLayout,VkImageLayout dstFinalLayout)
{
    auto& srcTex = src.data;
    auto& dstTex = dst.data;

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,srcTex->allocatedImage->vk_image,
                                                                src.currentLayout,
                                                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,dstTex->allocatedImage->vk_image,
                                                                dst.currentLayout,
                                                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


    VkImageCopy copyRegion = {};
    copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.srcSubresource.mipLevel = 0;
    copyRegion.srcSubresource.baseArrayLayer = 0;
    copyRegion.srcSubresource.layerCount = 1;

    copyRegion.srcOffset = { 0, 0, 0 };

    copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.dstSubresource.mipLevel = 0;
    copyRegion.dstSubresource.baseArrayLayer = 0;
    copyRegion.dstSubresource.layerCount = 1;

    copyRegion.dstOffset = { 0, 0, 0 };

    copyRegion.extent = { dst.extent.GetVkExtent().width,dst.extent.GetVkExtent().height, 1 };

    vkCmdCopyImage(cmd,
                   srcTex->allocatedImage->vk_image,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstTex->allocatedImage->vk_image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,&copyRegion
                   );

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,srcTex->allocatedImage->vk_image,
                                                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                srcFinalLayout);
    src.currentLayout = srcFinalLayout;

    VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd,dstTex->allocatedImage->vk_image,
                                                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                dstFinalLayout);
    dst.currentLayout = dstFinalLayout;
}

void RDG::CommandList::Dispatch(int groupX, int groupY,int groupZ)
{
    vkCmdDispatch(cmd, groupX,groupY,groupZ);
}

void RDG::CommandList::DrawMeshTask(int groupCount)
{
    vkCmdDrawMeshTasksEXT(cmd,groupCount,1,1);
}

void RDG::CommandList::DrawInstances(const Mesh &mesh, int instanceCount)
{
    if(mesh.vertBuffer.vk_buffer==VK_NULL_HANDLE||mesh.indexBuffer.vk_buffer==VK_NULL_HANDLE)
    {
        return;
    }
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd,0,1,&mesh.vertBuffer.vk_buffer, offsets);

    vkCmdBindIndexBuffer(cmd,mesh.indexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
    //Draw
    vkCmdDrawIndexed(cmd, static_cast<uint32_t>(mesh.indexCount), instanceCount, 0, 0, 0);
}

void RDG::CommandList::AddBarrier(RDG::TextureInfo &texture, VkImageLayout dstLayout)
{
    VkImageMemoryBarrier2 imageBarrier;
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageBarrier.pNext = nullptr;

    bool isDepth = texture.usage&TextureUsage::Depth;

    if(isDepth)
    {
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    }else
    {
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    }

    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageBarrier.oldLayout = texture.currentLayout;
    imageBarrier.newLayout = dstLayout;


    VkImageAspectFlags aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;


    VkImageSubresourceRange subImage {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = texture.mipLevels;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = texture.arrayCount;


    imageBarrier.subresourceRange = subImage;
    imageBarrier.image = texture.data->allocatedImage->vk_image;

    VkDependencyInfo depInfo {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}
