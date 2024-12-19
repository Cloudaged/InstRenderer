
#include "RenderPass.h"
#include "../VulkanContext.h"

ResourceMap RenderPass::resourceMap;

RenderPass::RenderPass()
{

}

RenderPass::~RenderPass()
{

}


VkImageUsageFlags RenderPass::GetUsage(AttachmentUsage usage)
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

VkImageLayout RenderPass::GetLayout(AttachmentUsage usage)
{
    switch (usage)
    {
        case AttachmentUsage::Color:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case AttachmentUsage::TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case AttachmentUsage::TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case AttachmentUsage::Present:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        case AttachmentUsage::Depth:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case AttachmentUsage::ShadowMap:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

AttachmentState RenderPass::GetState(AttachmentOP op,AttachmentUsage usage)
{switch (op)
    {
        case AttachmentOP::WriteOnly:
        {
            if(usage==AttachmentUsage::Depth)
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
            }
            else if(usage==AttachmentUsage::ShadowMap)
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
            }
            else
            {
                return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                       VK_ATTACHMENT_STORE_OP_STORE,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
            }
        }
        case AttachmentOP::ReadAndWrite:
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
        default:
        {
            return AttachmentState{VK_ATTACHMENT_LOAD_OP_CLEAR,
                                   VK_ATTACHMENT_STORE_OP_STORE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        }
    }


}

void RenderPass::Build()
{
    SetupAttachments();

    std::vector<VkAttachmentDescription> attDescriptions;
    std::vector<VkImageView> views;
    std::vector<VkAttachmentReference> outputRefs;
    VkAttachmentReference depthRef = {};

    bool hasDepth = false;

    int refIndex = 0;
    for (auto& res:outputResource)
    {
        auto& att = res.resRef;
        if(att.usage==AttachmentUsage::Present)
        {
            BuildPresentFrame();
            return;
        }

        this->width = att.width;
        this->height = att.height;
        VkFormat format = att.format;

        AllocAttachmentResource(att);

        auto state = GetState(res.opt,att.usage);
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


        if(att.usage == AttachmentUsage::Depth||att.usage == AttachmentUsage::ShadowMap)
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

        views.push_back(att.data->allocatedImage.imageView);
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

    if(vkCreateRenderPass(VulkanContext::GetContext().device,&passCreateInfo, nullptr,&passHandle)!=VK_SUCCESS)
    {
        std::cout<<"failed to create pass\n";
    }

    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = passHandle;
    fbInfo.width = width;
    fbInfo.height = height;
    fbInfo.attachmentCount = outputResource.size();
    fbInfo.pAttachments = views.data();
    fbInfo.layers =1;

    if(vkCreateFramebuffer(VulkanContext::GetContext().device,&fbInfo, nullptr,&framebufferHandle)!=VK_SUCCESS)
    {
        std::cout<<"Failed to build FrameBuffer\n";
    }

    std::vector<std::string> names;
    for (auto& att:inputAttDes)
    {
        names.push_back(att.name);
    }
    InputAttachmentDes(names);

    if(!renderState.isInit)
    {
        SetupRenderState();
    }

    renderState.isInit = true;

}


void RenderPass::BuildPresentFrame()
{
    auto& res = outputResource[0];
    auto& att = res.resRef;

    std::vector<VkImageView> views;
    std::vector<VkAttachmentDescription> attDescriptions;
    std::vector<VkAttachmentReference> refs;
    int refIndex = 0;

    width = att.width;
    height = att.height;

    VkFormat format = att.format;
    VkImageLayout layout = GetLayout(att.usage);

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

    if(vkCreateRenderPass(VulkanContext::GetContext().device,&passCreateInfo, nullptr,&passHandle)!=VK_SUCCESS)
    {
        std::cout<<"failed to create pass\n";
    }

    VulkanContext::GetContext().presentManager.InitFrameData(passHandle,width,height);

}

void RenderPass::InputAttachmentDes(std::vector<std::string> names)
{
    if(renderState.isInit)
    {
        auto& device= VulkanContext::GetContext().device;
        vkFreeDescriptorSets(device,VulkanContext::GetContext().pool,1, &this->inputAttDesSet);
        vkDestroyDescriptorSetLayout(device, this->inputAttDesLayout, nullptr);
    }


    std::vector<VkDescriptorSetLayoutBinding> bindings(names.size());
    for (int i = 0; i < names.size(); ++i)
    {
        bindings[i].binding = i;
        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[i].descriptorCount= 1;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(VulkanContext::GetContext().device, &layoutInfo, nullptr,
                                    &this->inputAttDesLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }


    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &inputAttDesLayout;

    vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo,&inputAttDesSet);

    std::vector<VkWriteDescriptorSet> writes;
    //Descriptor

    std::vector<VkDescriptorImageInfo> imageInfos(names.size());
    for (int i = 0; i < names.size(); ++i)
    {
        if(resourceMap.count(names[i]) == 0)
        {
            std::cout<<"Can't match the attachment through name\n";
        }
        auto& att = resourceMap[names[i]];

        imageInfos[i].imageView = att.textureInfo->data->allocatedImage.imageView;
        imageInfos[i].imageLayout = att.textureInfo->data->allocatedImage.layout;
        imageInfos[i].sampler = att.textureInfo->data->sampler;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = inputAttDesSet;
        descriptorWrite.dstBinding = i;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfos[i];
        descriptorWrite.pTexelBufferView = nullptr;

        writes.push_back(descriptorWrite);

    }
    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);
}

void RenderPass::TransAttachmentLayout(VkCommandBuffer cmd)
{
    for (auto& res:outputResource)
    {
        auto state = GetState(res.opt,res.resRef.usage);
        if(res.resRef.currentLayout != state.initLayout)
        {
            VulkanContext::GetContext().bufferAllocator.TransitionImage(cmd, res.resRef.data->allocatedImage.vk_image, res.resRef.currentLayout, state.initLayout);
        }
    }
}

void RenderPass::UpdateRecordedLayout()
{
    for (auto& res:outputResource)
    {
        auto state = GetState(res.opt,res.resRef.usage);
        res.resRef.currentLayout=state.finalLayout;
    }
}

void RenderPass::AllocAttachmentResource(TextureInfo &attachment)
{
    if(!attachment.hasInit)
    {
        auto usage = GetUsage(attachment.usage);
        auto layout = GetLayout(attachment.usage);

        VkImageAspectFlagBits aspect;
        if(attachment.usage == AttachmentUsage::Depth || attachment.usage == AttachmentUsage::ShadowMap)
        {
            aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        } else
        {
            aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        auto img = new AllocatedImage(attachment.format,usage,VkExtent2D{(uint32_t)attachment.width,(uint32_t)attachment.height},1,aspect);
        img->layout = layout;
        attachment.data = std::make_shared<Texture>(*img);
        attachment.hasInit = true;
    }
}

void RenderPass::ClearRes()
{
    auto device = VulkanContext::GetContext().device;
    vkDeviceWaitIdle(device);
    //Clear Renderpass
    vkDestroyRenderPass(device,passHandle, nullptr);

    //Clear FrameBuffer
    if(framebufferHandle!=VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(device,framebufferHandle, nullptr);
    }

    inputAttDes.clear();
    outputResource.clear();
}





