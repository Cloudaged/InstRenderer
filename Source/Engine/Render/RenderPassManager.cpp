
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
    shadowPass->Execute(view);
    compositionPass->Execute();
    presentPass->Execute();
    VulkanContext::GetContext().presentManager.EndRecordCommand(cmd);
}



void RenderPassManager::Setup()
{
    this->geoPass = std::make_unique<GeoPass>(globalDescriptorData);
    geoPass->SetupAttachments();
    geoPass->Build();

    this->shadowPass = std::make_unique<ShadowPass>(globalDescriptorData);
    shadowPass->SetupAttachments();
    shadowPass->Build();

    this->compositionPass = std::make_unique<CompositionPass>(globalDescriptorData);
    compositionPass->SetupAttachments();
    compositionPass->Build();


    this->presentPass = std::make_unique<PresentPass>();
    presentPass->SetupAttachments();
    presentPass->Build();

}


void RenderPassManager::RecreateAllPass()
{
    geoPass->ClearRes();
    shadowPass->ClearRes();
    compositionPass->ClearRes();
    presentPass->ClearRes();
    ClearAtt();


    geoPass->SetupAttachments();
    shadowPass->SetupAttachments();
    compositionPass->SetupAttachments();
    presentPass->SetupAttachments();


    geoPass->Build();
    shadowPass->Build();
    compositionPass->Build();
    presentPass->Build();
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
