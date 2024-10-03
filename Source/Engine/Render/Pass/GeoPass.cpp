
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
    /*//DescriptorLayout
    //Global Layout
    renderState.InputGlobalDesLayout(globalData.globalDesLayout);

    std::vector<DescriptorBindingSlot> bindings(0);

    //Material Layout
    DescriptorBindingSlot b1{0,VK_SHADER_STAGE_FRAGMENT_BIT,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    bindings.push_back(b1);
    renderState.CreatePerMaterialLayout(bindings);

    bindings.clear();
    //per Obj layout
    DescriptorBindingSlot b2{0,VK_SHADER_STAGE_VERTEX_BIT,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
    bindings.push_back(b2);
    renderState.CreatePerObjLayout(bindings);
    //Pipeline
    renderState.CreatePipeline(PipelineType::Mesh,passHandle,outputAttDes.size(),{"D:\\code_lib\\AGProject\\InstRenderer\\Asset\\vert.spv","D:\\code_lib\\AGProject\\InstRenderer\\Asset\\frag.spv"});

    //Create perObj descriptor
    perData = {};
    renderState.CreatePerObjDescriptor(sizeof(GeoPassPerObjData));*/
}

glm::mat4 GeoPass::GetModelMatrixFromTrans(Transform trans)
{
    auto mat =  glm::translate(glm::mat4(1),trans.pos);
    mat = glm::scale(mat,trans.scale);
    mat = glm::rotate(mat,trans.rotation.x,{1,0,0});
    mat = glm::rotate(mat,trans.rotation.y,{0,1,0});
    mat = glm::rotate(mat,trans.rotation.z,{0,0,1});
    return mat;}
