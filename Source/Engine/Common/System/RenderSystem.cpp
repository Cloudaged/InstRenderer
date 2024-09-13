
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"

void RenderSystem::BeginSystem(RenderSysBeginInfo info)
{

    passManager.view = info.registry->view<Renderable>();
    passManager.globalDescriptorData = info.globalData;
    passManager.Setup();
    passManager.Build();

    materialManager.Setup(&passManager.geometryPass->renderState.materialLayout,info.registry->view<Renderable>());

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
