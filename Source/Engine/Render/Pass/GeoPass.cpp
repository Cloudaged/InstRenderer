
#include "GeoPass.h"
#include "../VulkanContext.h"
GeoPass::GeoPass(GlobalDescriptorData data): globalData(data)
{
}

void GeoPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;


    attachmentMap["Position"] = AttachmentDes{"Position", winWidth, winHeight,
                                               AttachmentUsage::Color,VK_FORMAT_R16G16B16A16_SFLOAT, &positionAttachment};

    attachmentMap["Normal"] = AttachmentDes{"Normal", winWidth, winHeight,
                                            AttachmentUsage::Color,VK_FORMAT_R16G16B16A16_SFLOAT, &normalAttachment};

    attachmentMap["BaseColor"] = AttachmentDes{"BaseColor", winWidth, winHeight,
                                              AttachmentUsage::Color,VK_FORMAT_R16G16B16A16_SFLOAT, &baseColorAttachment};

    attachmentMap["MetallicRoughness"] = AttachmentDes{"MetallicRoughness", winWidth, winHeight,
                                              AttachmentUsage::Color,VK_FORMAT_R16G16B16A16_SFLOAT, &mrAttachment};

    attachmentMap["Depth"] = AttachmentDes{"Depth",winWidth,winHeight,
                                           AttachmentUsage::Depth,VK_FORMAT_D32_SFLOAT,&depthAttachment};

    outputResource.push_back({attachmentMap["Position"], AttachmentOP::WriteOnly});
    outputResource.push_back({attachmentMap["Normal"], AttachmentOP::WriteOnly});
    outputResource.push_back({attachmentMap["BaseColor"], AttachmentOP::WriteOnly});
    outputResource.push_back({attachmentMap["MetallicRoughness"], AttachmentOP::WriteOnly});
    outputResource.push_back({attachmentMap["Depth"], AttachmentOP::WriteOnly});
}

void GeoPass::Execute(entt::view<entt::get_t<Renderable, Transform>> view)
{
    std::lock_guard<std::mutex> guard(Locker::Get().loadResourceMtx);

    auto presentM = VulkanContext::GetContext().presentManager;

    auto cmd = presentM.presentFrames[presentM.currentFrame].cmd;

    TransAttachmentLayout(cmd);


    VkExtent2D extent = VulkanContext::GetContext().windowExtent;


    std::vector<VkClearValue> clearValues(outputResource.size());
    clearValues[0].color =  {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].color = {{0.0f, 0.0f, 0.0f, 0}};
    clearValues[2].color =  {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[3].color =  {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[4].depthStencil = {1.0f, 0};

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
    for (auto& entity:view)
    {
        auto renderComponents = view.get<Renderable>(entity);
        auto transComponents = view.get<Transform>(entity);

        perData = {transComponents.globalTransform};

        //Update
        memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(perObjDesBuffer),
               &perData,
               sizeof(perData));

        vkCmdPushConstants(cmd,renderState.pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(glm::mat4),&perData);
        std::vector<VkDescriptorSet> sets = {globalData.globalDes,renderComponents.material->set,perObjDes,inputAttDesSet};
        //Bind
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

void GeoPass::SetupRenderState()
{
    //DescriptorLayout
    //Global Layout
    InputGlobalDesLayout();

    std::vector<DescriptorBindingSlot> bindings(0);

    //Material layout
    DescriptorBindingSlot slotBuffer{0,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
    DescriptorBindingSlot baseColor{1,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    DescriptorBindingSlot normal{2,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    DescriptorBindingSlot roughnessMetallic{3,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};

    bindings.push_back(slotBuffer);
    bindings.push_back(baseColor);
    bindings.push_back(normal);
    bindings.push_back(roughnessMetallic);

    CreatePerMaterialLayout(bindings);
    bindings.clear();

    //per Obj layout
    DescriptorBindingSlot b2{0,VK_SHADER_STAGE_VERTEX_BIT,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
    bindings.push_back(b2);
    CreatePerObjLayout(bindings);

    //Input layout
    renderState.layouts[3] = inputAttDesLayout;

    //Pipeline
    renderState.CreatePipeline(PipelineType::Mesh,
                               passHandle, outputResource.size() - 1,
                               {FILE_PATH("Asset/Shader/slangSPV/GeoVert.slang.spv"),
                                FILE_PATH("Asset/Shader/slangSPV/GeoFrag.slang.spv")});

    //Create perObj descriptor
    CreatePerObjDescriptor(sizeof(GeoPassPerObjData));
}

/*glm::mat4 GeoPass::GetModelMatrixFromTrans(Transform trans)
{
    auto mat =  glm::translate(glm::mat4(1),trans.pos);
    mat = glm::scale(mat,trans.scale);
    mat = glm::rotate(mat,trans.rotation.x,{1,0,0});
    mat = glm::rotate(mat,trans.rotation.y,{0,1,0});
    mat = glm::rotate(mat,trans.rotation.z,{0,0,1});
    return mat;
}*/

void GeoPass::InputGlobalDesLayout()
{
    this->globalLayout = globalData.globalDesLayout;
    renderState.layouts[0] = this->globalLayout;
}

void GeoPass::CreatePerMaterialLayout(std::vector<DescriptorBindingSlot> bindings)
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
                                    &materialLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }
    renderState.layouts[1] = materialLayout;
}

void GeoPass::CreatePerObjLayout(std::vector<DescriptorBindingSlot> bindings)
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
                                    &perObjLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }
    renderState.layouts[2]=perObjLayout;
}

void GeoPass::CreatePerObjDescriptor(size_t uniformSize)
{
    //CreateBuffer
    perObjDesBuffer = *VulkanContext::GetContext().bufferAllocator.CreateBuffer(uniformSize,VK_BUFFER_USAGE_2_UNIFORM_BUFFER_BIT_KHR,VMA_MEMORY_USAGE_CPU_ONLY);

    //Allocate
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &perObjLayout;

    vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo,&perObjDes);

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = perObjDesBuffer.vk_buffer;
    bufferInfo.range = uniformSize;
    bufferInfo.offset = 0;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = perObjDes;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,1,&descriptorWrite,0, nullptr);
}
