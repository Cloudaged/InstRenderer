
#include "RenderGraph.h"
#include "../VulkanContext.h"

RDG::RenderGraph renderGraph;
namespace RDG
{
    void RenderGraph::DeclareResource()
    {
        uint32_t shadowMapWidth = 2048;
        uint32_t shadowMapHeight = 2048;

        uint32_t winWidth = (uint32_t)VulkanContext::GetContext().windowExtent.width;
        uint32_t winHeight = (uint32_t)VulkanContext::GetContext().windowExtent.height;

        resourceMap["Position"] = {.type = ResourceType::Texture,
                .textureInfo =TextureInfo{{winWidth, winHeight},
                                          AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["Normal"] = {.type = ResourceType::Texture,
                .textureInfo = TextureInfo{{winWidth, winHeight},
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["BaseColor"] = {.type = ResourceType::Texture,
                .textureInfo = TextureInfo{{winWidth, winHeight},
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["MetallicRoughness"] = {.type = ResourceType::Texture,
                .textureInfo = TextureInfo{{winWidth, winHeight},
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["Depth"] = {.type = ResourceType::Texture,
                .textureInfo = TextureInfo{{winWidth, winHeight},
                                           AttachmentUsage::Depth, VK_FORMAT_D32_SFLOAT}};

        resourceMap["ShadowMap"] = {.type = ResourceType::Texture,
                .textureInfo = TextureInfo{{winWidth, winHeight},
                                           AttachmentUsage::ShadowMap, VK_FORMAT_D32_SFLOAT}};

        resourceMap["Lighted"] = {.type = ResourceType::Texture,
                                .textureInfo = TextureInfo{{winWidth, winHeight},
                                AttachmentUsage::Color, VK_FORMAT_R8G8B8A8_SRGB}};

        resourceMap["Present"] = {.type = ResourceType::Texture,
                .textureInfo = TextureInfo{{winWidth, winHeight},
                                            AttachmentUsage::Present, VK_FORMAT_B8G8R8A8_SRGB}};

    }

    void RenderGraph::DeclarePass()
    {
        passMap["Geometry"] = {.type = RenderPassType::Graphic,
                               .input = {},
                               .output = {"Position","Normal","BaseColor","MetallicRoughness","Depth"},
                               .pipeline = {PipelineType::Mesh,"GeometryVert.spv","GeometryFrag.spv"},
                               .executeFunc = [&]()
                               {
                                    int b = 6;
                               }};

        passMap["Shadow"] =    {.type =  RenderPassType::Graphic,
                                .input = {"Position"},
                                .output = {"ShadowMap"},
                                .pipeline = {PipelineType::Mesh,"ShadowMapVert.spv","ShadowMapFrag.spv"},
                                .executeFunc = [&]()
                                {
                                    int b = 6;
                                }};

        passMap["Composition"] = {.type =  RenderPassType::Graphic,
                                  .input = {"Position","Normal","BaseColor","MetallicRoughness"},
                                    .output = {"Lighted"},
                                    .pipeline = {PipelineType::RenderQuad,"CompositionVert.spv","CompositionFrag.spv"},
                                    .executeFunc = [&]()
                                    {
                                        int b = 6;
                                    }};

        passMap["Present"] = {.type =  RenderPassType::Present,
                                .input = {"Lighted"},
                                .output = {},
                                .pipeline = {PipelineType::RenderQuad,"CompositionVert.spv","CompositionFrag.spv"},
                                .executeFunc = [&]()
                                {
                                    int b = 6;
                                }};
    }

    void RenderGraph::Compile()
    {
        WriteDependency();
        CreateResource();
        CreateRenderPass();

    }

    void RenderGraph::Execute()
    {

    }

    void RenderGraph::WriteDependency()
    {
        //Write Resources' producer
        for (auto& [passName,passData] :passMap)
        {
            for (auto& resName : passData.output)
            {
                resourceMap[resName].producerPass = passName;
            }
        }
        //Write Passes' producer
        for (auto& [passName,passData] : passMap)
        {
            for (auto& resName : passData.input)
            {
                passData.producers.push_back(resourceMap[resName].producerPass);
            }
        }
    }

    void RenderGraph::CreateResource()
    {
        for (auto& [resName,resData] : resourceMap)
        {
            if(IsImageType(resData.type)&&resData.textureInfo.has_value())
            {
                CreateImageResource(resData);
            }

            if(IsBufferType(resData.type)&&resData.bufferInfo.has_value())
            {
                CreateBufferResource(resData);
            }
        }
    }

    void RenderGraph::CreateImageResource(ResourceRef& resource)
    {
        auto& texture = resource.textureInfo->data;
        auto& textureInfo = resource.textureInfo;
        auto usage = GetImageUsage(textureInfo->usage);
        auto aspect = GetAspectFlag(textureInfo->usage);

        auto img = AllocatedImage(textureInfo->format,usage,textureInfo->extent,1,aspect);
        texture = std::make_shared<Texture>(std::move(img));
    }

    void RenderGraph::CreateBufferResource(ResourceRef& resource)
    {
        auto& buffer = resource.bufferInfo->data;
        auto usage = GetBufferUsage(resource.type);
        auto b  = VulkanContext::GetContext().bufferAllocator.CreateBuffer(resource.bufferInfo->size,usage,VMA_MEMORY_USAGE_CPU_ONLY);
        buffer = std::shared_ptr<Buffer>(b);
    }

    void RenderGraph::CreateRenderPass()
    {
        for (auto& [passName,passData] : passMap)
        {
            if(passData.type==RenderPassType::Graphic)
            {
                CreateGraphicPass(passData);
            }else if(passData.type==RenderPassType::Present)
            {
                CreatePresentPass(passData);
            }else if(passData.type==RenderPassType::Compute)
            {
                CreateComputePass(passData);
            }
        }
    }

    void RenderGraph::InsertBarrier()
    {

    }

    void RenderGraph::CreateGraphicPass(PassRef& passData)
    {
        std::vector<VkAttachmentDescription> attDescriptions;
        std::vector<VkImageView> views;
        std::vector<VkAttachmentReference> outputRefs;
        VkAttachmentReference depthRef = {};

        bool hasDepth = false;

        uint32_t width;
        uint32_t height;

        int refIndex = 0;
        for (auto& resName : passData.output)
        {
            auto& res = resourceMap[resName];
            auto& att = res.textureInfo;

            width = att->extent.width;
            height = att->extent.height;
            VkFormat format = att->format;


            auto state = GetImageState(att->usage);
            //Attachment
            VkAttachmentDescription attachmentDes{};
            attachmentDes.format =format;
            attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDes.loadOp = state.loadOp;
            attachmentDes.storeOp = state.storeOp;
            attachmentDes.initialLayout = state.initLayout;
            attachmentDes.finalLayout = state.finalLayout;


            if(att->usage == AttachmentUsage::Depth||att->usage == AttachmentUsage::ShadowMap)
            {
                depthRef.attachment = refIndex;
                depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                hasDepth = true;
            } else
            {
                VkAttachmentReference ref{};
                ref.attachment = refIndex;
                ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                outputRefs.push_back(ref);
            }
            attDescriptions.push_back(attachmentDes);

            views.push_back(att->data->allocatedImage.imageView);
            refIndex++;
        }
        //RenderPass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = outputRefs.size();
        subpass.pColorAttachments = outputRefs.data();
        subpass.inputAttachmentCount = 0;
        if(hasDepth)
        {
            subpass.pDepthStencilAttachment = &depthRef;
        } else
        {
            subpass.pDepthStencilAttachment = nullptr;
        }

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo passCreateInfo{};
        passCreateInfo.sType =VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        passCreateInfo.attachmentCount = attDescriptions.size();
        passCreateInfo.pAttachments = attDescriptions.data();
        passCreateInfo.subpassCount = 1;
        passCreateInfo.pSubpasses = &subpass;
        passCreateInfo.dependencyCount = 1;
        passCreateInfo.pDependencies = &dependency;

        if(vkCreateRenderPass(VulkanContext::GetContext().device,&passCreateInfo, nullptr,&passData.data.passHandle)!=VK_SUCCESS)
        {
            std::cout<<"failed to create pass\n";
        }

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = passData.data.passHandle;
        fbInfo.width = width;
        fbInfo.height = height;
        fbInfo.attachmentCount = passData.output.size();
        fbInfo.pAttachments = views.data();
        fbInfo.layers =1;

        if(vkCreateFramebuffer(VulkanContext::GetContext().device,&fbInfo, nullptr,&passData.data.framebufferHandle)!=VK_SUCCESS)
        {
            std::cout<<"Failed to build FrameBuffer\n";
        }
    }

    void RenderGraph::CreateComputePass(PassRef& passData)
    {

    }

    void RenderGraph::CreatePresentPass(PassRef& passData)
    {
        std::string attName = *passData.output.begin();
        TextureInfo& att = resourceMap[attName].textureInfo.value();

        std::vector<VkImageView> views;
        std::vector<VkAttachmentDescription> attDescriptions;
        std::vector<VkAttachmentReference> refs;
        int refIndex = 0;

        uint32_t width = att.extent.width;
        uint32_t height = att.extent.height;

        VkFormat format = att.format;

        //Attachment
        VkAttachmentDescription attachmentDes{};
        attachmentDes.format =format;
        attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attDescriptions.push_back(attachmentDes);

        refIndex++;


        //RenderPass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = refs.data();

        std::array<VkSubpassDependency, 2> dependencies{};

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo passCreateInfo{};
        passCreateInfo.sType =VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        passCreateInfo.attachmentCount = attDescriptions.size();
        passCreateInfo.pAttachments = attDescriptions.data();
        passCreateInfo.subpassCount = 1;
        passCreateInfo.pSubpasses = &subpass;
        passCreateInfo.dependencyCount = dependencies.size();
        passCreateInfo.pDependencies = dependencies.data();

        if(vkCreateRenderPass(VulkanContext::GetContext().device,&passCreateInfo, nullptr,&passData.data.passHandle)!=VK_SUCCESS)
        {
            std::cout<<"failed to create pass\n";
        }

        VulkanContext::GetContext().presentManager.InitFrameData(passData.data.passHandle,width,height);

    }

    void RenderGraph::CreateDescriptor()
    {

    }

    AttachmentState GetImageState(AttachmentUsage usage)
    {
        if(usage==AttachmentUsage::Depth)
        {
            return AttachmentState{VK_ATTACHMENT_LOAD_OP_LOAD,
                                   VK_ATTACHMENT_STORE_OP_STORE,
                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
        }else if (usage==AttachmentUsage::ShadowMap)
        {
            return AttachmentState{VK_ATTACHMENT_LOAD_OP_LOAD,
                                   VK_ATTACHMENT_STORE_OP_STORE,
                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
        }
        else
        {
            return AttachmentState{VK_ATTACHMENT_LOAD_OP_LOAD,
                                   VK_ATTACHMENT_STORE_OP_STORE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        }
    }


    VkImageUsageFlags GetImageUsage(AttachmentUsage usage)
    {
        switch (usage)
        {
            case AttachmentUsage::Color:
                return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
            case AttachmentUsage::TransferSrc:
                return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            case AttachmentUsage::TransferDst:
                return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            case AttachmentUsage::Present:
                return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            case AttachmentUsage::Depth:
                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            case AttachmentUsage::ShadowMap:
                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            default:
                return VK_IMAGE_USAGE_SAMPLED_BIT;
        }
    }

    VkBufferUsageFlagBits GetBufferUsage(ResourceType type)
    {
        if(type==ResourceType::Uniform)
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if(type==ResourceType::SSBO)
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    }

    VkImageAspectFlagBits GetAspectFlag(AttachmentUsage usage)
    {
        if(usage==AttachmentUsage::Depth||usage==AttachmentUsage::ShadowMap)
        {
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        } else
        {
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }


    bool IsImageType(ResourceType type)
    {

    }
    bool IsBufferType(ResourceType type)
    {

    }

} // RDG