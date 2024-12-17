
#include "RenderPassManager.h"

#include <memory>
#include "iostream"
#include "VulkanContext.h"
RenderPassManager::RenderPassManager()
{

}

void RenderPassManager::Setup(entt::view<entt::get_t<Renderable, Transform>> view, GlobalDescriptorData globalData)
{
    this->view = std::move(view);
    this->globalDescriptorData = std::move(globalData);

    this->geoPass = std::make_unique<GeoPass>(globalDescriptorData);
    geoPass->Build();

    this->shadowPass = std::make_unique<ShadowPass>(globalDescriptorData);
    shadowPass->Build();

    /*this->blockerSearchPass = std::make_unique<BlockerSearchPass>(globalDescriptorData);
    blockerSearchPass->Build();*/

    this->compositionPass = std::make_unique<CompositionPass>(globalDescriptorData);
    compositionPass->Build();

    this->presentPass = std::make_unique<PresentPass>();
    presentPass->Build();
}

void RenderPassManager::ExecuteAllPass()
{
    auto cmd = VulkanContext::GetContext().presentManager.BeginRecordCommand();
    geoPass->Execute(view);
    shadowPass->Execute(view);
   // blockerSearchPass->Execute();
    compositionPass->Execute();
    presentPass->Execute();
    VulkanContext::GetContext().presentManager.EndRecordCommand(cmd);
}

void RenderPassManager::RecreateAllPass()
{
    geoPass->ClearRes();
    shadowPass->ClearRes();
    //blockerSearchPass->ClearRes();
    compositionPass->ClearRes();
    presentPass->ClearRes();
    ClearAtt();


    geoPass->Build();
    shadowPass->Build();
   // blockerSearchPass->Build();
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
        auto& data = (att.second.data);
        vkDestroyImageView(device,data->allocatedImage.imageView, nullptr);
        vkDestroyImage(device,data->allocatedImage.vk_image, nullptr);
        vkDestroySampler(device,data->sampler, nullptr);
    }
    RenderPass::attachmentMap.clear();
}

void RenderPassManager::DeclaresResource()
{

}

