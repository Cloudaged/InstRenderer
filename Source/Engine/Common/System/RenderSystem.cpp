
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"

void RenderSystem::BeginSystem(RenderSysBeginInfo info)
{
    passManager.view = info.registry->view<Renderable,Transform>();
    passManager.globalDescriptorData = info.globalData;
    passManager.Setup();
    passManager.Build();
    materialManager.Setup(&passManager.geoPass->materialLayout,info.registry->view<Renderable>());
    VulkanContext::GetContext().passManager = &this->passManager;
}

void RenderSystem::Execute()
{
    VulkanContext::GetContext().DrawPrepare();
    passManager.ExecuteAllPass();
    VulkanContext::GetContext().Submit();
}

RenderSystem::RenderSystem()
{
    passManager = RenderPassManager();
}

void RenderSystem::RecreatePassRes()
{
    std::cout<<"reCreateD\n";
    passManager.RecreateAllPass();
}
