
#include "PresentPass.h"
#include "../VulkanContext.h"

PresentPass::PresentPass()
{
    int winWidth = VulkanContext::GetContext().windowExtent.width;
    int winHeight = VulkanContext::GetContext().windowExtent.height;

    inputAttDes.push_back(attachmentMap["Lighted"]);

    attachmentMap["Present"] = AttachmentDes{"Present",winWidth,winHeight,
                                             AttachmentUsage::Present,AttachmentOP::DontCare,
                                             VK_FORMAT_B8G8R8A8_SRGB, false, nullptr};

    outputAttDes.push_back(attachmentMap["Present"]);
}

void PresentPass::SetupAttachments()
{

}

void PresentPass::Execute()
{

}

void PresentPass::SetupRenderState()
{
    InputAttachmentDes({"Lighted"});
    //Pipeline
    renderState.CreatePipeline(PipelineType::RenderQuad,passHandle,outputAttDes.size(),
                               {FILE_PATH("Asset/Shader/spv/Comp.vert.spv"),
                                FILE_PATH("Asset/Shader/spv/Comp.frag.spv")});
    renderState.isInit = true;
}
