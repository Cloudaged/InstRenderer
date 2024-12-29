
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
        if(VulkanContext::GetContext().isResize)
            return false;

        curPass = std::make_shared<PassRef>(passRef);

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
                    clearValues[i].color={1.0, 0, 0, 0};
                    clearValues[i].depthStencil = {0,0};
                }
            } else
            {
                for (int i = 0; i < passRef.output.size() - 1; ++i)
                {
                    clearValues[i].color = {1.0, 0, 0, 0};
                    clearValues[i].depthStencil = {0,0};
                }
                clearValues[passRef.output.size()-1].color ={0.0f, 0,0,0};
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
        vkCmdEndRenderPass(cmd);
    }

    void CommandList::PushConstantsForHandles(void* data)
    {
        vkCmdPushConstants(cmd,curPass->pipeline.pipelineLayout,VK_SHADER_STAGE_ALL,0,curPass->pipeline.handleSize,data);
    }

    void CommandList::BindDescriptor()
    {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                curPass->pipeline.pipelineLayout,
                                0, 1,&VulkanContext::GetContext().bindlessSet,0, nullptr);
    }

    void CommandList::BindPipeline()
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, curPass->pipeline.pipeline);
    }

    void CommandList::DrawMesh(const Mesh& mesh)
    {
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd,0,1,&mesh.vertBuffer.vk_buffer, offsets);

        vkCmdBindIndexBuffer(cmd,mesh.indexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
        //Draw
        vkCmdDrawIndexed(cmd,static_cast<uint32_t>(mesh.indexSize),1,0,0,0);
    }

    void CommandList::DrawRenderQuad()
    {
        vkCmdDraw(cmd, 3, 1, 0, 0);
    }


} // RDG