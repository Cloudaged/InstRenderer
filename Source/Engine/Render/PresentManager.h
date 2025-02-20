
#ifndef INSTRENDERER_PRESENTMANAGER_H
#define INSTRENDERER_PRESENTMANAGER_H
#include "volk.h"
#include "vector"
#include "RenderGraph/RenderResource.h"
#include "../Common/Core/Locker.h"
#include <functional>
struct PresentFrame
{
    VkCommandBuffer cmd;
    VkFramebuffer framebuffer;

    VkSemaphore swapChainSemaphore,renderSemaphore;
    VkFence renderFence;
};

#define INFLIGHT_COUNT 3

class PresentManager
{
public:
    PresentManager(){};
    void InitFrameData(VkRenderPass renderPass,int width,int height);
    VkCommandBuffer BeginRecordCommand();
    void EndRecordCommand(VkCommandBuffer cmd);
    std::vector<PresentFrame> presentFrames;
    void RecreateSwapChain();
public:
    std::function<void()> recreatePassFunc;
    uint32_t currentFrame=0;
    uint32_t lastFrame = 0;
    uint32_t swapChainImageIndex;
    VkRenderPass lastRenderpass;
    std::vector<VkImage> swapchainImage;
    std::vector<VkImageView> swapchainView;
private:
    void InitSyncStructures();
    void ClearSwapChain();
};


#endif //INSTRENDERER_PRESENTMANAGER_H
