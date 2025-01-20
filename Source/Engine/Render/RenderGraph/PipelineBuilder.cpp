
#include "PipelineBuilder.h"
#include "../VulkanContext.h"
namespace RDG
{
    std::vector<char> PipelineBuilder::ReadFile(const std::string &fileName)
    {
        std::ifstream file(fileName,std::ios::ate | std::ios::binary);

        if(!file.is_open())
        {
            throw std::runtime_error("failed to open Shader!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(),fileSize);
        file.close();
        return buffer;
    }

    VkShaderModule PipelineBuilder::LoadShaderData(std::string path)
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

    void PipelineBuilder::CreatePipeline(PipelineRef& pipelineRef,int attCount,VkRenderPass renderPass)
    {
        PipelineSettings settings{};

        if(pipelineRef.type==PipelineType::RayTracing)
        {
            CreateRayTracingPipeline(pipelineRef,attCount,renderPass);
            return;
        }else
        {
            switch (pipelineRef.type)
            {
                case PipelineType::Mesh:
                    settings = {true, true, true, VK_CULL_MODE_BACK_BIT};
                    break;
                case PipelineType::RenderQuad:
                    settings = {true, false, false, VK_CULL_MODE_FRONT_BIT};
                    break;
                case PipelineType::Skybox:
                    settings = {true, false, true, VK_CULL_MODE_FRONT_BIT};
                    break;
                default:
                    settings = {true, true, true, VK_CULL_MODE_BACK_BIT};
                    break;
            }
        }

        VkShaderModule vertModule =LoadShaderData(GetShaderFullPath(pipelineRef.rsShaders.vert));
        VkShaderModule fragModule = LoadShaderData(GetShaderFullPath(pipelineRef.rsShaders.frag));

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

        if(settings.hasVertAtt)
        {
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attriDescription.size();
            vertexInputInfo.pVertexBindingDescriptions =&bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attriDescription.data();
        } else
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        }

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

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentsVector(attCount,colorBlendAttachment);

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_NO_OP;
        colorBlending.attachmentCount = attCount;
        colorBlending.pAttachments = colorBlendAttachmentsVector.data();

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = settings.depthTest;
        depthStencil.depthWriteEnable = settings.depthWrite;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthBoundsTestEnable =VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};


        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount= 1;
        pipelineLayoutInfo.pSetLayouts = &VulkanContext::GetContext().bindlessLayout;
        VkPushConstantRange pushConstantRange{};
        if(pipelineRef.handleSize!=0)
        {
            pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
            pushConstantRange.offset = 0;
            pushConstantRange.size = pipelineRef.handleSize;

            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        }else
        {
            std::cout<<"This pass don't have handle pushConstants\n";
        }

        if(vkCreatePipelineLayout(VulkanContext::GetContext().device,&pipelineLayoutInfo, nullptr,&pipelineRef.pipelineLayout) != VK_SUCCESS)
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
        pipelineInfo.layout = pipelineRef.pipelineLayout;
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
                                     &pipelineRef.pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }

    std::string PipelineBuilder::GetShaderFullPath(std::string name)
    {
        std::string nameWithExtension = name+".slang.spv";
        return FILE_PATH("Asset/Shader/slangSPV/"+nameWithExtension);
    }

    void PipelineBuilder::CreateRayTracingPipeline(PipelineRef& pipelineRef,int attCount,VkRenderPass renderPass)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &VulkanContext::GetContext().bindlessLayout;

        VkPushConstantRange pushConstantRange{};
        if(pipelineRef.handleSize!=0)
        {
            pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
            pushConstantRange.offset = 0;
            pushConstantRange.size = pipelineRef.handleSize;

            pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
            pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
        }else
        {
            std::cout<<"This pass don't have handle pushConstants\n";
        }

        if(vkCreatePipelineLayout(VulkanContext::GetContext().device,&pipelineLayoutCreateInfo, nullptr,&pipelineRef.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("fail to create RT pipeline layout");
        }


        enum RTStageIndices : uint32_t
        {
            Gen = 0,
            Miss = 2,
            ClosetHit = 1,
            ShaderGroupCount = 3
        };

        std::vector<VkShaderModule> modules =
                {
                    LoadShaderData(GetShaderFullPath(pipelineRef.rtShaders.gen)),
                    LoadShaderData(GetShaderFullPath(pipelineRef.rtShaders.chit)),
                    LoadShaderData(GetShaderFullPath(pipelineRef.rtShaders.miss))
                };

        std::vector<VkPipelineShaderStageCreateInfo> stages(ShaderGroupCount);
        VkPipelineShaderStageCreateInfo stage{.sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,.pName = "main"};
        //Gen
        stage.module = modules[Gen];
        stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        stages[Gen] = stage;

        //Miss
        stage.module = modules[Miss];
        stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        stages[Miss] = stage;

        //cHit
        stage.module = modules[ClosetHit];
        stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        stages[ClosetHit] = stage;

        VkRayTracingShaderGroupCreateInfoKHR group{VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR};
        group.anyHitShader       = VK_SHADER_UNUSED_KHR;
        group.closestHitShader   = VK_SHADER_UNUSED_KHR;
        group.generalShader      = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;

        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        group.generalShader = Gen;
        pipelineRef.sbt.groups.push_back(group);

        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        group.closestHitShader = ClosetHit;
        pipelineRef.sbt.groups.push_back(group);

        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.generalShader = Miss;
        pipelineRef.sbt.groups.push_back(group);



        VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
        rayPipelineInfo.stageCount = stages.size();
        rayPipelineInfo.pStages = stages.data();
        rayPipelineInfo.groupCount =  pipelineRef.sbt.groups.size();
        rayPipelineInfo.pGroups =  pipelineRef.sbt.groups.data();
        rayPipelineInfo.maxPipelineRayRecursionDepth = 1;
        rayPipelineInfo.layout = pipelineRef.pipelineLayout;
        auto device = VulkanContext::GetContext().device;
        vkCreateRayTracingPipelinesKHR(device,{},{},1,&rayPipelineInfo, nullptr,&pipelineRef.pipeline);


        for(auto& s : stages)
        {
            vkDestroyShaderModule(device, s.module, nullptr);
        }

        VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rtProps{};
        rtProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rtProps;

        vkGetPhysicalDeviceProperties2(VulkanContext::GetContext().gpu,&deviceProperties2);

        pipelineRef.sbt.shaderHandleSize = rtProps.shaderGroupHandleSize;
        uint32_t handleAlignment = rtProps.shaderGroupHandleAlignment;
        uint32_t baseAligment = rtProps.shaderGroupBaseAlignment;

        auto AlignUp = [](uint32_t value, uint32_t alignment) {
            return (value + alignment - 1) & ~(alignment - 1);
        };

        uint32_t handleSizeAligned = AlignUp(pipelineRef.sbt.shaderHandleSize,handleAlignment);

        pipelineRef.sbt.genRegion.stride = handleSizeAligned;
        pipelineRef.sbt.genRegion.size =  handleSizeAligned;

        pipelineRef.sbt.missRegion.stride = handleSizeAligned;
        pipelineRef.sbt.missRegion.size = handleSizeAligned;

        pipelineRef.sbt.hitRegion.stride = handleSizeAligned;
        pipelineRef.sbt.hitRegion.size = handleSizeAligned;

        //Get Handle
        uint32_t handleCount = 3;
        uint32_t handleSize =pipelineRef.sbt.genRegion.size+pipelineRef.sbt.missRegion.size+pipelineRef.sbt.hitRegion.size;
        std::vector<uint8_t> handles(handleSize);
        if(vkGetRayTracingShaderGroupHandlesKHR(device,pipelineRef.pipeline,0,handleCount,handleSize,handles.data())!=VK_SUCCESS)
        {
            std::cout<<"Failed to Get group Handles\n";
        };


        //CreateBuffer
        pipelineRef.sbt.genBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(pipelineRef.sbt.shaderHandleSize,
                                                                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                                                             | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
                                                                                             VMA_MEMORY_USAGE_CPU_TO_GPU);
        VkBufferDeviceAddressInfo deviceAddressInfoG{};
        deviceAddressInfoG.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        deviceAddressInfoG.buffer = pipelineRef.sbt.genBuffer->vk_buffer;
        pipelineRef.sbt.genRegion.deviceAddress = vkGetBufferDeviceAddress(device,&deviceAddressInfoG);


        pipelineRef.sbt.missBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(pipelineRef.sbt.shaderHandleSize,
                                                                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                                                             | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
                                                                                             VMA_MEMORY_USAGE_CPU_TO_GPU);
        VkBufferDeviceAddressInfo deviceAddressInfoM{};
        deviceAddressInfoM.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        deviceAddressInfoM.buffer = pipelineRef.sbt.missBuffer->vk_buffer;
        pipelineRef.sbt.missRegion.deviceAddress = vkGetBufferDeviceAddress(device,&deviceAddressInfoM);


        pipelineRef.sbt.hitBuffer = VulkanContext::GetContext().bufferAllocator.CreateBuffer(pipelineRef.sbt.shaderHandleSize,
                                                                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                                                             | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
                                                                                             VMA_MEMORY_USAGE_CPU_TO_GPU);
        VkBufferDeviceAddressInfo deviceAddressInfoH{};
        deviceAddressInfoH.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        deviceAddressInfoH.buffer = pipelineRef.sbt.hitBuffer->vk_buffer;
        pipelineRef.sbt.hitRegion.deviceAddress = vkGetBufferDeviceAddress(device,&deviceAddressInfoH);

        auto getHandle = [&] (int i) { return handles.data() + i * pipelineRef.sbt.shaderHandleSize; };

        uint8_t* genMappedData =(uint8_t*)VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*pipelineRef.sbt.genBuffer);
        uint8_t* hitMappedData =(uint8_t*)VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*pipelineRef.sbt.hitBuffer);
        uint8_t* missMappedData =(uint8_t*)VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*pipelineRef.sbt.missBuffer);

        uint32_t handleIndex = 0;

        uint8_t* pData = (uint8_t*)genMappedData;
        memcpy(pData, getHandle(handleIndex++), pipelineRef.sbt.shaderHandleSize);

        pData = hitMappedData;
        memcpy(pData,getHandle(handleIndex++),pipelineRef.sbt.shaderHandleSize);

        pData = missMappedData;
        memcpy(pData,getHandle(handleIndex++),pipelineRef.sbt.shaderHandleSize);



    }
}