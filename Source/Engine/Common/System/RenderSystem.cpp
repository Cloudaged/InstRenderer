
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"

void RenderSystem::BeginSystem(RenderSysBeginInfo info)
{
    passManager.Setup(info.registry->view<Renderable,Transform>(),info.globalData);
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

