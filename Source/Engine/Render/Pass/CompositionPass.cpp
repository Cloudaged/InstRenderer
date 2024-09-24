
#include "CompositionPass.h"
#include "../VulkanContext.h"

void CompositionPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    AttachmentDes presentAtt{};
    presentAtt.name = "Present";
    presentAtt.width = winWidth;
    presentAtt.height = winHeight;
    presentAtt.usage = AttachmentUsage::Present;
    presentAtt.op = AttachmentOP::DontCare;
    presentAtt.format = VK_FORMAT_B8G8R8A8_SRGB;
    presentAtt.data = &this->presentData;

    attDes.push_back(presentAtt);

}

void CompositionPass::Execute(entt::view<entt::get_t<Renderable,Transform>> view)
{
    auto presentM = VulkanContext::GetContext().presentManager;

    auto cmd = presentM.BeginRecordCommand();

    VkExtent2D extent = VulkanContext::GetContext().windowExtent;

    VkClearValue renderClearValue = {0.0,0.0,0.0,1.0};
    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = passHandle;
    beginInfo.framebuffer = presentM.presentFrames[presentM.currentFrame].framebuffer;
    beginInfo.renderArea.offset ={0,0};
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount =1;
    beginInfo.pClearValues = &renderClearValue;

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
    for (auto& entity:view)
    {
        auto renderComponents = view.get<Renderable>(entity);
        auto transComponents = view.get<Transform>(entity);

        perData = {GetModelMatrixFromTrans(transComponents)};
        //Update
        memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(renderState.perObjDesBuffer),
               &perData,
               sizeof(perData));
        std::vector<VkDescriptorSet> sets = {globalData.globalDes,renderComponents.material.set,renderState.perObjDes};
        //Bind
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                renderState.pipelineLayout,
                                0, sets.size(),sets.data(),0, nullptr);

        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(cmd,0,1,&renderComponents.mesh.vertBuffer.vk_buffer, offsets);

        vkCmdBindIndexBuffer(cmd,renderComponents.mesh.indexBuffer.vk_buffer,0,VK_INDEX_TYPE_UINT32);
        //Draw
        vkCmdDrawIndexed(cmd,static_cast<uint32_t>(renderComponents.mesh.index.size()),1,0,0,0);
    }
    vkCmdEndRenderPass(cmd);
    presentM.EndRecordCommand(cmd);
}

void CompositionPass::SetupRenderState()
{
    //DescriptorLayout
    //Global Layout
    renderState.InputGlobalDesLayout(globalData.globalDesLayout);

    std::vector<DescriptorBindingSlot> bindings(0);

    //Material Layout
    DescriptorBindingSlot b1{0,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    bindings.push_back(b1);
    renderState.CreatePerMaterialLayout(bindings);

    bindings.clear();
    //per Obj layout
    DescriptorBindingSlot b2{0,VK_SHADER_STAGE_VERTEX_BIT,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
    bindings.push_back(b2);
    renderState.CreatePerObjLayout(bindings);
    //Pipeline
    renderState.CreatePipeline(PipelineType::Mesh,passHandle,attDes.size(),{"D:\\code_lib\\AGProject\\InstRenderer\\Asset\\vert.spv","D:\\code_lib\\AGProject\\InstRenderer\\Asset\\frag.spv"});

    //Create perObj descriptor
    perData = {};
    renderState.CreatePerObjDescriptor(sizeof(CompPassPerObjData));

}

CompositionPass::CompositionPass(GlobalDescriptorData data): globalData(data)
{

}

glm::mat4 CompositionPass::GetModelMatrixFromTrans(Transform trans)
{
    auto mat =  glm::translate(glm::mat4(1),trans.pos);
    mat = glm::scale(mat,trans.scale);
    mat = glm::rotate(mat,trans.rotation.x,{1,0,0});
    mat = glm::rotate(mat,trans.rotation.y,{0,1,0});
    mat = glm::rotate(mat,trans.rotation.z,{0,0,1});
    return mat;
}


