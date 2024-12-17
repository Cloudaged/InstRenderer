
#include "PrefilterPass.h"
#include "../VulkanContext.h"
PrefilterPass::PrefilterPass()
{

}

void PrefilterPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    resourceMap["Prefiltered"] = TextureInfo{"Prefiltered", winWidth, winHeight,
                                             AttachmentUsage::Prefiltered, VK_FORMAT_R8G8B8A8_SRGB, nullptr};

    outputResource.push_back({resourceMap["Prefiltered"], AttachmentOP::WriteOnly});
}

void PrefilterPass::Execute()
{

}

void PrefilterPass::SetupRenderState()
{
    
}
