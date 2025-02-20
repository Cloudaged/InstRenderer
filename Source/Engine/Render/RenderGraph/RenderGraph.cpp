#include "RenderGraph.h"
#include "../VulkanContext.h"
#include "PipelineBuilder.h"
namespace RDG
{
    RenderGraph::RenderGraph()
    {
    }

    void RenderGraph::DeclareResource(std::shared_ptr<Scene> scene)
    {

    }


    void RenderGraph::Compile()
    {
        CreateResource();
        WriteDependency();
        CreateRenderPass();
        CreateDescriptor();
        CreateVkPipeline();
    }

    void RenderGraph::Execute()
    {
        if(VulkanContext::GetContext().isResize)
        {
            return;
        }
        commandList.BeginCommand();
        for (auto& passData : passArr)
        {
            TransitionLayout(commandList,passData);
            commandList.BeginRenderPass(passData);
            commandList.BindPipeline();
            commandList.BindDescriptor();
            passData.executeFunc(commandList);
            commandList.EndRenderPass();
            InsertBarrier(commandList,passData);
        }
        commandList.EndCommand();
    }

    void RenderGraph::WriteDependency()
    {

    }

    void RenderGraph::CreateResource()
    {
        for (auto& [resHandle,resData] : resourceMap)
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
        auto &texture = resource.textureInfo->data;
        if(texture.get()!= nullptr)
        {
            return;
        }
        auto &textureInfo = resource.textureInfo;
        textureInfo->currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //auto usage = GetImageUsage(textureInfo->usage);
        //auto aspect = GetAspectFlag(textureInfo->usage);
        ImageType imageType;
        if (IsDepthType(textureInfo->usage))
        {
            imageType = ImageType::Depth;
        } else
        {
            imageType = ImageType::Color;
        }
        auto img = std::make_shared<AllocatedImage>(imageType,textureInfo->format,textureInfo->usage,
                                                    textureInfo->extent.GetVkExtent(),
                                                    textureInfo->mipLevels,textureInfo->arrayCount);
        texture = std::make_shared<Texture>(img);
    }

    void RenderGraph::CreateBufferResource(ResourceRef& resource)
    {
        auto& buffer = resource.bufferInfo->data;
        auto usage = GetBufferUsage(resource.type);
        auto b  = VulkanContext::GetContext().bufferAllocator.CreateBuffer(resource.bufferInfo->size,usage,VMA_MEMORY_USAGE_CPU_ONLY);
        buffer = std::shared_ptr<Buffer>(b);
        resource.bufferInfo.value().mappedAddress = VulkanContext::GetContext().bufferAllocator.GetMappedMemory(*buffer);
    }

    void RenderGraph::CreateRenderPass()
    {
        for (auto& passData : passArr)
        {
            if(passData.type==RenderPassType::Raster)
            {
                CreateGraphicPass(passData);
            }else if(passData.type==RenderPassType::Present)
            {
                CreatePresentPass(passData);
            }else if(passData.type==RenderPassType::Compute)
            {
                CreateComputePass(passData);
            }else if(passData.type == RenderPassType::RayTracing)
            {

            }
        }
    }

    void RenderGraph::CreateGraphicPass(PassRef& passData)
    {
        std::vector<VkAttachmentDescription> attDescriptions;
        std::vector<VkImageView> views;
        std::vector<VkAttachmentReference> outputRefs;
        VkAttachmentReference depthRef = {};

        VkExtent2D attExtent;

        int refIndex = 0;
        bool multiView = false;
        int layerCount = 1;
        for (auto& resHandle : passData.output)
        {
            auto& res = resourceMap[resHandle];
            if(!res.textureInfo.has_value())
            {
                std::cout<<"There is no textureInfo\n";
                continue;
            }
            auto& att = res.textureInfo.value();

            attExtent = att.extent.GetVkExtent();
            VkFormat format = att.format;

            bool isRW = false;
            if(std::find(passData.input.begin(),passData.input.end(),resHandle) != passData.input.end())
            {
                isRW = true;
            } else
            {
                isRW = false;
            }

            if(att.arrayCount!=1)
            {
                multiView = true;
                layerCount = att.arrayCount;
            }


            auto state = GetImageState(att.usage,isRW);
            passData.attDes.push_back({att,state});
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


            if(att.usage == TextureUsage::Depth || att.usage == TextureUsage::ShadowMap)
            {
                depthRef.attachment = refIndex;
                depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                passData.hasDepth = true;
            } else
            {
                VkAttachmentReference ref{};
                ref.attachment = refIndex;
                ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                outputRefs.push_back(ref);
            }
            attDescriptions.push_back(attachmentDes);

            views.push_back(att.data->allocatedImage->imageView);
            refIndex++;
        }
        //RenderPass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = outputRefs.size();
        subpass.pColorAttachments = outputRefs.data();
        subpass.inputAttachmentCount = 0;
        if(passData.hasDepth)
        {
            subpass.pDepthStencilAttachment = &depthRef;
        } else
        {
            subpass.pDepthStencilAttachment = nullptr;
        }
        uint32_t  viewMask = (1<<CASCADED_COUNT)-1;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassMultiviewCreateInfo multiviewInfo{};
        multiviewInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
        multiviewInfo.subpassCount = 1;
        multiviewInfo.pViewMasks = &viewMask;
        multiviewInfo.correlationMaskCount = 1;
        multiviewInfo.pCorrelationMasks = &viewMask;

        VkRenderPassCreateInfo passCreateInfo{};
        passCreateInfo.sType =VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        passCreateInfo.attachmentCount = attDescriptions.size();
        passCreateInfo.pAttachments = attDescriptions.data();
        passCreateInfo.subpassCount = 1;
        passCreateInfo.pSubpasses = &subpass;
        passCreateInfo.dependencyCount = 1;
        passCreateInfo.pDependencies = &dependency;

        if(multiView)
        {
            passCreateInfo.pNext = &multiviewInfo;
        }
        if(vkCreateRenderPass(VulkanContext::GetContext().device,&passCreateInfo, nullptr,&passData.data.passHandle)!=VK_SUCCESS)
        {
            std::cout<<"failed to create pass\n";
        }

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = passData.data.passHandle;
        fbInfo.width = attExtent.width;
        fbInfo.height = attExtent.height;
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
        Handle attHandle;
        for (const auto& handle:passData.output)
        {
            attHandle = handle;
        }


        std::vector<VkImageView> views;
        std::vector<VkAttachmentDescription> attDescriptions;
        int refIndex = 0;

        uint32_t width = VulkanContext::GetContext().windowExtent.width;
        uint32_t height = VulkanContext::GetContext().windowExtent.height;

        VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;

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

        std::vector<VkAttachmentReference> refs;
        refs.push_back({0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});

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
        for (auto& [resHandle,resData]:resourceMap)
        {
            if(IsBufferType(resData.type))
            {
                WriteBufferDescriptor(resData);
            }else if(IsImageType(resData.type))
            {
                WriteImageDescriptor(resData);
            }else if(resData.type==ResourceType::Accleration)
            {
                WriteAccelerationSTDescriptor(resData);
            }
        }
    }

    void RenderGraph::WriteImageDescriptor(const ResourceRef& resource)
    {
        if(!resource.textureInfo.has_value())
        {
            std::cout<<"Failed to load texture\n";
            return;
        }

        if(resource.textureInfo.value().usage == TextureUsage::Depth)
            return;

        auto data = resource.textureInfo.value().data;
        if(data== nullptr)
        {
            std::cout<<"Write an unallocated res\n";
        }
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageView = data->allocatedImage->imageView;
        imageInfo.sampler = data->sampler;


        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        if(resource.type == ResourceType::StorageImage)
        {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            write.dstBinding = STORE_IMAGE_BINDING;
        }else
        {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.dstBinding = TEXTURE_BINDING;
        }
        write.dstSet = VulkanContext::GetContext().bindlessSet;
        write.descriptorCount = 1;
        write.dstArrayElement = resource.handle;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(VulkanContext::GetContext().device, 1, &write, 0, nullptr);

    }

    void RenderGraph::WriteBufferDescriptor(const ResourceRef& resource)
    {
        if(!resource.bufferInfo.has_value())
        {
            std::cout<<"Failed to load buffer\n";
            return;
        }
        auto data = resource.bufferInfo.value().data;
        if(data== nullptr)
        {
            std::cout<<"Write an unallocated res\n";
        }
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = data->vk_buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = resource.bufferInfo.value().size;

        VkWriteDescriptorSet write{};
        if(resource.type==ResourceType::SSBO)
        {
            write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }else if(resource.type==ResourceType::Uniform)
        {
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        if(resource.type==ResourceType::Uniform)
        {
            write.dstBinding = UBO_BINDING;
        } else if(resource.type == ResourceType::SSBO)
        {
            write.dstBinding = SSBO_BINDING;
        }
        write.dstSet = VulkanContext::GetContext().bindlessSet;
        write.descriptorCount = 1;
        write.dstArrayElement = resource.handle;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(VulkanContext::GetContext().device, 1, &write, 0, nullptr);

    }

    void RenderGraph::WriteAccelerationSTDescriptor(const ResourceRef &resource)
    {
        VkWriteDescriptorSetAccelerationStructureKHR write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        write.accelerationStructureCount = 1;
        write.pAccelerationStructures = &resource.rtScene->topAS.accelerationStructure;
        accelerationStructure = std::make_shared<ResourceRef>(resource);

        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.dstSet = VulkanContext::GetContext().bindlessSet;
        writeDescriptor.dstBinding = ACCELERATION_ST_BINDING;
        writeDescriptor.dstArrayElement = resource.handle;
        writeDescriptor.descriptorCount = 1;
        writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        writeDescriptor.pNext = &write;

        vkUpdateDescriptorSets(VulkanContext::GetContext().device, 1, &writeDescriptor, 0, nullptr);
    }


    void RenderGraph::CreateVkPipeline()
    {
        for (auto& passData : passArr)
        {
            int attCount;
            if(passData.type==RenderPassType::Present)
            {
                attCount = 1;
            } else
            {
                attCount = passData.output.size();
                if(passData.hasDepth)
                {
                    attCount--;
                }
            }

            PipelineBuilder::CreatePipeline(passData.pipeline,attCount,passData.data.passHandle);

        }
    }

    Handle RenderGraph::AddResource(ResourceRef&& resource)
    {
        Handle handle = handleAllocator.Allocate();
        resource.handle = handle;
        resourceMap[handle] = std::move(resource);
        return handle;
    }

    void RenderGraph::AddPass(const PassRef &pass)
    {
        passArr.push_back(std::move(pass));
    }

    Handle RenderGraph::GetResourceHandle(std::string name)
    {
        for (const auto& [resHandle,resData]:resourceMap)
        {
            if(resData.name==name)
            {
                return resHandle;
            }
        }
        std::cout<<"Can't find"+name+"\n";
        return 0;
    }

    void RenderGraph::InsertBarrier(CommandList &cmd, const PassRef &passRef)
    {
        for (auto& att : passRef.attDes)
        {
            auto& textureInfo = att.texInfo;
            att.texInfo.currentLayout = att.state.finalLayout;
            cmd.AddBarrier(textureInfo,att.state.finalLayout);


        }
    }

    void RenderGraph::TransitionLayout(CommandList &cmd, const PassRef &passRef)
    {
        for (auto& att : passRef.attDes)
        {
            auto& textureInfo = att.texInfo;
            if(textureInfo.currentLayout!=att.state.initLayout)
            {
                cmd.AddBarrier(textureInfo,att.state.initLayout);
                textureInfo.currentLayout = att.state.initLayout;
            }

        }
    }

    Handle RenderGraph::AddOuterResource(ResourceRef&& resource)
    {
        Handle handle = handleAllocator.Allocate();
        resource.handle = handle;
        if(IsBufferType(resource.type))
        {
            WriteBufferDescriptor(resource);
        }else if(IsImageType(resource.type))
        {
            WriteImageDescriptor(resource);
        }
        resourceMap[handle] = std::move(resource);

        return handle;
    }

    void RenderGraph::RecreateAllPass()
    {
        for (auto& pass:passArr)
        {
            RecreatePassResource(pass);
        }
        CreateRenderPass();
    }

    void RenderGraph::RecreatePassResource(PassRef &pass)
    {
        //Clear Pass
        auto device = VulkanContext::GetContext().device;
        vkDeviceWaitIdle(device);
        vkDestroyRenderPass(device,pass.data.passHandle, nullptr);
        //Clear FrameBuffer
        if(pass.data.framebufferHandle!=VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(device,pass.data.framebufferHandle, nullptr);
        }
        //Clear and recreate resource
//        for (auto& output:pass.output)
//        {
//            auto& resource = resourceMap[output];
//            if(IsImageType(resource.type)&&resource.textureInfo.has_value())
//            {
//                if(resource.textureInfo->extent == WINDOW_EXTENT &&
//                   resource.textureInfo->usage != AttachmentUsage::Present)
//                {
//                    //Clear
//                    auto& data = resource.textureInfo->data;
//                    data.reset();
//                    //Recreate
//                    CreateImageResource(resource);
//                    WriteImageDescriptor(resource);
//                }
//            }
//        }

        for (auto& pair:resourceMap)
        {
            auto& resource = pair.second;
            if(IsImageType(resource.type)&&resource.textureInfo.has_value())
            {
                if(resource.textureInfo->extent == WINDOW_EXTENT &&
                   resource.textureInfo->usage != TextureUsage::Present)
                {
                    //Clear
                    auto& data = resource.textureInfo->data;
                    data.reset();
                    //Recreate
                    CreateImageResource(resource);
                    WriteImageDescriptor(resource);
                }
            }
        }
    }

    void RenderGraph::CreateRayTracingPass(PassRef &passData)
    {

    }



    AttachmentState GetImageState(UsageFlags usage, bool isRW)
    {

        if(usage & TextureUsage::Depth)
        {
            if(isRW)
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_LOAD,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
            } else
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
            }

//        }else if (usage & TextureUsage::ShadowMap)
//        {
//            if(isRW)
//            {
//                return AttachmentState{VK_ATTACHMENT_LOAD_OP_LOAD,
//                                       VK_ATTACHMENT_STORE_OP_STORE,
//                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
//                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
//            } else
//            {
//                return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
//                                       VK_ATTACHMENT_STORE_OP_STORE,
//                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
//            }
        }else if(usage & TextureUsage::Storage)
        {
            return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                   VK_ATTACHMENT_STORE_OP_STORE,
                                   VK_IMAGE_LAYOUT_GENERAL,
                                   VK_IMAGE_LAYOUT_GENERAL};
        }
        else
        {
            if(isRW)
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_LOAD,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
            } else
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
            }
        }
    }


//    VkImageUsageFlags GetImageUsage(UsageFlags usage)
//    {
//        switch ((uint32_t)usage.to_ulong())
//        {
//            case TextureUsage::Color:
//                return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//            case TextureUsage::TransferSrc:
//                return VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
//            case TextureUsage::TransferDst:
//                return VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
//            case TextureUsage::Present:
//                return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//            case TextureUsage::Depth:
//                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
//            case TextureUsage::ShadowMap:
//                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
//            case TextureUsage::Storage:
//                return VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
//            default:
//                return VK_IMAGE_USAGE_SAMPLED_BIT;
//        }
//    }

    VkBufferUsageFlagBits GetBufferUsage(ResourceType type)
    {
        if(type==ResourceType::Uniform)
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if(type==ResourceType::SSBO)
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    }

    VkImageAspectFlagBits GetAspectFlag(UsageFlags usage)
    {
        if(usage&TextureUsage::Depth || usage&TextureUsage::ShadowMap)
        {
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        } else
        {
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    bool IsDepthType(UsageFlags usage)
    {
        if((usage & TextureUsage::Depth))
        {
            return true;
        } else
        {
            return false;
        }
    }



    bool IsImageType(ResourceType type)
    {
        if(type==ResourceType::Texture||type==ResourceType::StorageImage)
        {
            return true;
        } else
        {
            return false;
        }
    }
    bool IsBufferType(ResourceType type)
    {
        if(type==ResourceType::Uniform||type==ResourceType::SSBO)
        {
            return true;
        } else
        {
            return false;
        }
    }

} // RDG