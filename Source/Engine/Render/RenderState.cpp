
#include "RenderState.h"

#include "VulkanContext.h"

std::vector<char> ReadFile(const std::string &fileName)
{
    std::ifstream file(fileName,std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(),fileSize);
    file.close();
    return buffer;
}

void RenderState::CreatePerMaterialLayout(std::vector<DescriptorBindingSlot> bindings)
{
    std::vector<VkDescriptorSetLayoutBinding> b(bindings.size());

    VkDescriptorSetLayout layout;

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
                                    &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }
    materialLayout = layout;
    layouts.push_back(layout);
}

void RenderState::CreatePipeline(PipelineType type,VkRenderPass renderPass,int attachmentCount,ShaderPath path)
{
    PipelineSettings settings{};

    switch (type)
    {
        case PipelineType::Mesh:
            settings = {true, true,VK_CULL_MODE_BACK_BIT};
    }

    VkShaderModule vertModule =LoadShaderData(path.vertPath);
    VkShaderModule fragModule = LoadShaderData(path.fragPath);

    std::vector<VkDynamicState> dynamicStates =
            {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR
            };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attriDescription = Vertex::GetAttributeDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attriDescription.size();

    vertexInputInfo.pVertexBindingDescriptions =&bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attriDescription.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable =VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = settings.cullMode;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,fragShaderStageInfo};


    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask =VK_COLOR_COMPONENT_R_BIT |
                                         VK_COLOR_COMPONENT_G_BIT |
                                         VK_COLOR_COMPONENT_B_BIT |
                                         VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentsVector(attachmentCount,colorBlendAttachment);

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlending.attachmentCount = attachmentCount;
    colorBlending.pAttachments = colorBlendAttachmentsVector.data();

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = settings.depthTest;
    depthStencil.depthWriteEnable = settings.depthWrite;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable =VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};


    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount= layouts.size();
    pipelineLayoutInfo.pSetLayouts = layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if(vkCreatePipelineLayout(VulkanContext::GetContext().device,&pipelineLayoutInfo, nullptr,&pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("fail to create pipeline layout");
    }

    //shaders
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    //fix function
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    //layout
    pipelineInfo.layout = pipelineLayout;
    //render Pass
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    //depth test
    pipelineInfo.pDepthStencilState = &depthStencil;
    //other pipeline settings
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(VulkanContext::GetContext().device,VK_NULL_HANDLE,
                                 1,&pipelineInfo, nullptr,
                                 &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

RenderState::RenderState()
{

}

VkShaderModule RenderState::LoadShaderData(std::string path)
{
    auto buffer = ReadFile(path);
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

    VkShaderModule sm;
    if (vkCreateShaderModule(VulkanContext::GetContext().device, &createInfo, nullptr, &sm) != VK_SUCCESS)
    {
        std::cout << "Failed to read shader\n";
    }
    return sm;
}

void RenderState::InputGlobalDesLayout(VkDescriptorSetLayout layout)
{
    globalLayout = layout;
    layouts.push_back(layout);
}

void RenderState::CreatePerObjLayout(std::vector<DescriptorBindingSlot> bindings)
{

    std::vector<VkDescriptorSetLayoutBinding> b(bindings.size());

    VkDescriptorSetLayout layout;

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
                                    &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }
    perObjLayout=layout;
    layouts.push_back(layout);
}


void RenderState::CreatePerObjDescriptor(size_t uniformSize)
{
    //CreateBuffer
    perObjDesBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(uniformSize,VK_BUFFER_USAGE_2_UNIFORM_BUFFER_BIT_KHR,VMA_MEMORY_USAGE_CPU_ONLY);

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



