
#include "RenderPassManager.h"
#include "iostream"
#include "VulkanContext.h"
RenderPassManager::RenderPassManager()
{

}

void RenderPassManager::ExecuteAllPass()
{
    auto cmd = VulkanContext::GetContext().presentManager.BeginRecordCommand();
    geoPass->Execute(view);
    compositionPass->Execute();
    VulkanContext::GetContext().presentManager.EndRecordCommand(cmd);
}

void RenderPassManager::Build()
{
    geoPass->Build();
    compositionPass->Build();
}

void RenderPassManager::Setup()
{
    this->geoPass = new GeoPass(globalDescriptorData);
    geoPass->SetupAttachments();

    this->compositionPass = new CompositionPass();
    compositionPass->SetupAttachments();
}


void RenderPassManager::RecreateAllPass()
{
    geoPass->ClearRes();
    compositionPass->ClearRes();
    ClearAtt();


    geoPass->SetupAttachments();
    compositionPass->SetupAttachments();

    geoPass->Build();
    compositionPass->Build();


}

void RenderPassManager::ClearAtt()
{
    //Clear Attachment
    auto& device = VulkanContext::GetContext().device;
    for (auto& att:RenderPass::attachmentMap)
    {
        if(att.first=="Present")
            continue;
        auto& data = *(att.second.data);
        vkDestroyImageView(device,data->allocatedImage.imageView, nullptr);
        vkDestroyImage(device,data->allocatedImage.vk_image, nullptr);
        vkDestroySampler(device,data->sampler, nullptr);
    }
    RenderPass::attachmentMap.clear();
}
