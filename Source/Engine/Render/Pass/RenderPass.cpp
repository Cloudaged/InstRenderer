
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
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

void RenderPass::Build()
{
    std::vector<VkAttachmentDescription> attDescriptions;

    std::vector<VkAttachmentReference> inputRefs;
    std::vector<VkAttachmentReference> outputRefs;
    std::vector<VkImageView> views;

    int refIndex = 0;
    //Input
    for (auto& att:inputAttDes)
    {
        //Get data
        width = att.width;
        height = att.height;
        VkFormat format = att.format;
        VkImageLayout layout = GetLayout(att.usage);
        auto lsop = GetLSOP(att.op);
        //Get data(need descriptor)

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

        inputRefs.push_back(ref);

        attDescriptions.push_back(attachmentDes);
        refIndex++;
    }

    //Output
    for (auto& att:outputAttDes)
    {
        if(att.usage==AttachmentUsage::Present)
        {
            BuildPresentFrame();
            return;
        }

        //Get data
        width = att.width;
        height = att.height;
        VkFormat format = att.format;
        VkImageUsageFlagBits usage = GetUsage(att.usage);
        VkImageLayout layout = GetLayout(att.usage);
        auto lsop = GetLSOP(att.op);

        //Allocate image
        *att.data = new AllocatedImage(format,usage,VkExtent2D{width,height},1,VK_IMAGE_ASPECT_COLOR_BIT);
        auto data = *att.data;

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
        outputRefs.push_back(ref);

        attDescriptions.push_back(attachmentDes);

        views.push_back(data->imageView);
        refIndex++;
    }
    //RenderPass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = outputRefs.size();
    subpass.pColorAttachments = outputRefs.data();
    subpass.inputAttachmentCount = inputRefs.size();
    subpass.pInputAttachments = inputRefs.data();

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
    fbInfo.attachmentCount = outputAttDes.size();
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
    auto& att = outputAttDes[0];

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



