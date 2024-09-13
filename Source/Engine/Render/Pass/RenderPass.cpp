
#include "RenderPass.h"
#include "../VulkanContext.h"
VkImageUsageFlagBits RenderPass::GetUsage(AttachmentUsage usage)
{
    switch (usage)
    {
        case AttachmentUsage::ShaderReadOnly:
            return VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        case AttachmentUsage::ColorAttachment:
            return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        case AttachmentUsage::TransferSrc:
            return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        case AttachmentUsage::TransferDst:
            return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        case AttachmentUsage::Present:
            return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        default:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
    }
}

VkImageLayout RenderPass::GetLayout(AttachmentUsage usage)
{
    switch (usage)
    {
        case AttachmentUsage::ShaderReadOnly:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case AttachmentUsage::ColorAttachment:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case AttachmentUsage::TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case AttachmentUsage::TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case AttachmentUsage::Present:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
}

void RenderPass::Build()
{
    std::vector<VkImageView> views;
    std::vector<VkAttachmentDescription> attDescriptions;
    std::vector<VkAttachmentReference> refs;
    int refIndex = 0;
    for (auto& att:attDes)
    {
        if(att.usage==AttachmentUsage::Present)
        {
            BuildPresentFrame();
            return;
        }

        width = att.width;
        height = att.height;

        VkFormat format = att.format;
        VkImageUsageFlagBits usage = GetUsage(att.usage);
        VkImageLayout layout = GetLayout(att.usage);
        auto lsop = GetLSOP(att.op);

        //Attachment
        VkAttachmentDescription attachmentDes{};
        attachmentDes.format =format;
        attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDes.loadOp = lsop.loadOp;
        attachmentDes.storeOp = lsop.storeOp;
        attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDes.finalLayout = layout;

        VkAttachmentReference ref{};
        ref.attachment = refIndex;
        if(!att.isDepthBuffer)
        {
            ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else
        {
            ref.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        }
        refs.push_back(ref);

        attDescriptions.push_back(attachmentDes);

        refIndex++;

        //Frame Buffer
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = att.format;
        imageCreateInfo.extent.width = att.width;
        imageCreateInfo.extent.height = att.height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        imageCreateInfo.usage = usage;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VmaAllocation allocation;
        VmaAllocationInfo info;

        if(vmaCreateImage(VulkanContext::GetContext().allocator,&imageCreateInfo, &allocCreateInfo,&att.data->image,&allocation ,
                          &info)!=VK_SUCCESS)
        {
            std::cout<<"Failed to create Image in FrameBuffer\n";
        }

        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = att.data->image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = att.format;
        if(!att.isDepthBuffer)
        {
            viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        } else
        {
            viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(VulkanContext::GetContext().device,&viewCreateInfo, nullptr,&att.data->view)!=VK_SUCCESS)
        {
            std::cout<<"Failed to Create view in FrameBuffer\n";
        }

        views.push_back(att.data->view);
    }
    //RenderPass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = refs.data();

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
    fbInfo.attachmentCount = attDes.size();
    fbInfo.pAttachments = views.data();
    fbInfo.layers =1;

    if(vkCreateFramebuffer(VulkanContext::GetContext().device,&fbInfo, nullptr,&framebufferHandle)!=VK_SUCCESS)
    {
        std::cout<<"Failed to build FrameBuffer\n";
    }
    SetupRenderState();

}

RenderPass::LoadStoreOP RenderPass::GetLSOP(AttachmentOP op)
{
    switch (op)
    {
        case AttachmentOP::ReadOnly:
            return {VK_ATTACHMENT_LOAD_OP_LOAD,VK_ATTACHMENT_STORE_OP_DONT_CARE};
        case AttachmentOP::ReadAndWrite:
            return {VK_ATTACHMENT_LOAD_OP_LOAD,VK_ATTACHMENT_STORE_OP_STORE};
        case AttachmentOP::WriteOnly:
            return{VK_ATTACHMENT_LOAD_OP_CLEAR,VK_ATTACHMENT_STORE_OP_STORE};
        default:
            return {VK_ATTACHMENT_LOAD_OP_DONT_CARE,VK_ATTACHMENT_STORE_OP_DONT_CARE};
    }

}

void RenderPass::BuildPresentFrame()
{
    auto& att = attDes[0];

    std::vector<VkImageView> views;
    std::vector<VkAttachmentDescription> attDescriptions;
    std::vector<VkAttachmentReference> refs;
    int refIndex = 0;

    width = att.width;
    height = att.height;

    VkFormat format = att.format;
    VkImageUsageFlagBits usage = GetUsage(att.usage);
    VkImageLayout layout = GetLayout(att.usage);
    auto lsop = GetLSOP(att.op);

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

    VkAttachmentReference ref{};
    ref.attachment = refIndex;
    if(!att.isDepthBuffer)
    {
        ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    } else
    {
        ref.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    }
    refs.push_back(ref);

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

    SetupRenderState();
}



