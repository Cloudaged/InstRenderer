
#include "PresentPass.h"
#include "../VulkanContext.h"

PresentPass::PresentPass()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    attachmentMap["Present"] = AttachmentDes{"Present",winWidth,winHeight,
                                             AttachmentUsage::Present,AttachmentOP::DontCare,
                                             VK_FORMAT_B8G8R8A8_SRGB, false, nullptr};
}

void PresentPass::SetupAttachments()
{

}

void PresentPass::Execute()
{

}

void PresentPass::SetupRenderState()
{

}
