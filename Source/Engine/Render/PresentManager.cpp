
#include "PresentManager.h"
#include "VulkanContext.h"
void PresentManager::InitFrameData(VkRenderPass renderPass,int width,int height)
{
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VmaAllocation allocation;
    VmaAllocationInfo info;

    this->lastRenderpass = renderPass;

    presentFrames.resize(INFLIGHT_COUNT);

    for (int i = 0; i < presentFrames.size(); ++i)
    {
        //Cmd buffer
        VkCommandBufferAllocateInfo cmdInfo{};
        cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdInfo.pNext = nullptr;

        cmdInfo.commandPool = VulkanContext::GetContext().gameCmdPool;
        cmdInfo.commandBufferCount = 1;

        cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        if (vkAllocateCommandBuffers(VulkanContext::GetContext().device, &cmdInfo, &presentFrames[i].cmd) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        std::vector<VkImageView> views =
                {
                    swapchainView[i]
                };

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = renderPass;
        fbInfo.width = width;
        fbInfo.height = height;
        fbInfo.attachmentCount = views.size();
        fbInfo.pAttachments = views.data();
        fbInfo.layers =1;

        if (vkCreateFramebuffer(VulkanContext::GetContext().device, &fbInfo, nullptr, &presentFrames[i].framebuffer) !=
            VK_SUCCESS)
        {
            std::cout << "Failed to build FrameBuffer\n";
        }
    }

    InitSyncStructures();
}

void PresentManager::InitSyncStructures()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < INFLIGHT_COUNT; ++i)
    {
        vkCreateSemaphore(VulkanContext::GetContext().device,&semaphoreInfo, nullptr,&presentFrames[i].swapChainSemaphore);
        vkCreateSemaphore(VulkanContext::GetContext().device,&semaphoreInfo, nullptr,&presentFrames[i].renderSemaphore);
        vkCreateFence(VulkanContext::GetContext().device,&fenceInfo, nullptr,&presentFrames[i].renderFence);
    }

}

VkCommandBuffer PresentManager::BeginRecordCommand()
{
    VkCommandBufferBeginInfo cmdBeginInfo{};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    cmdBeginInfo.pInheritanceInfo = nullptr;

    VkCommandBuffer cmd = presentFrames[currentFrame].cmd;
    if(vkBeginCommandBuffer(cmd,&cmdBeginInfo)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command allocatedBuffer!");
    }
    return cmd;
}

void PresentManager::EndRecordCommand(VkCommandBuffer cmd)
{
    vkEndCommandBuffer(cmd);

}

void PresentManager::RecreateSwapChain()
{
    std::lock_guard<std::mutex> lock(Locker::Get().resizeMtx);

    SwapChainSupportDetails swapChainSupportDetails = VulkanFuncs::QuerySwapChainSupport(VulkanContext::GetContext().gpu);
    VkExtent2D extent = VulkanFuncs::ChooseSwapExtent(swapChainSupportDetails.capabilities);
    while (extent.width==0||extent.height==0)
    {
        SwapChainSupportDetails swapChainSupportDetails = VulkanFuncs::QuerySwapChainSupport(VulkanContext::GetContext().gpu);
        extent = VulkanFuncs::ChooseSwapExtent(swapChainSupportDetails.capabilities);
        SDL_WaitEvent(VulkanContext::GetContext().sdlEvent);
    }
    vkDeviceWaitIdle(VulkanContext::GetContext().device);

    ClearSwapChain();
    VulkanContext::GetContext().CreateSwapchain();
    extent = VulkanContext::GetContext().windowExtent;
    InitFrameData(lastRenderpass,(uint32_t)extent.width,(uint32_t)extent.height);
    recreatePassFunc();
    //VulkanContext::GetContext().passManager->RecreateAllPass();
    VulkanContext::GetContext().isResize = false;

}

void PresentManager::ClearSwapChain()
{
    const auto& device = VulkanContext::GetContext().device;
    for (int i = 0; i < presentFrames.size(); ++i)
    {
        vkDestroyFramebuffer(device,presentFrames[i].framebuffer, nullptr);
        vkDestroySemaphore(device,presentFrames[i].renderSemaphore, nullptr);
        vkDestroySemaphore(device,presentFrames[i].swapChainSemaphore, nullptr);
        vkDestroyFence(device,presentFrames[i].renderFence, nullptr);
    }

    for (int i = 0; i < swapchainView.size(); ++i)
    {
        vkDestroyImageView(device,swapchainView[i], nullptr);
    }

    vkDestroySwapchainKHR(device,
                          VulkanContext::GetContext().swapchainData.swapchain,
                          nullptr);

}
