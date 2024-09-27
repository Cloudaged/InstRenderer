
#include "GeoPass.h"
#include "../VulkanContext.h"
GeoPass::GeoPass(GlobalDescriptorData data)
{
}

void GeoPass::SetupAttachments()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;



    attachmentMap["BaseColor"] = AttachmentDes{winWidth,winHeight,
                                               AttachmentUsage::ColorAttachment,AttachmentOP::WriteOnly,
                                               VK_FORMAT_R8G8B8A8_SRGB, false,&baseColorAttachment};

    attachmentMap["Normal"] = AttachmentDes{winWidth,winHeight,
                                               AttachmentUsage::ColorAttachment,AttachmentOP::WriteOnly,
                                               VK_FORMAT_R8G8B8A8_SRGB, false,&normalAttachment};

    attachmentMap["Position"] = AttachmentDes{winWidth,winHeight,
                                            AttachmentUsage::ColorAttachment,AttachmentOP::WriteOnly,
                                            VK_FORMAT_R8G8B8A8_SRGB, false,&positionAttachment};

    outputAttDes.push_back(attachmentMap["BaseColor"]);
    outputAttDes.push_back(attachmentMap["Normal"]);
    outputAttDes.push_back(attachmentMap["Position"]);
}

void GeoPass::Execute(entt::view<entt::get_t<Renderable, Transform>> view)
{

}

void GeoPass::SetupRenderState()
{
    //Layout


    //Pipeline

    //Descriptor
}
