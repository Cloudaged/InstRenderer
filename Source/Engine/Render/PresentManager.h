
#ifndef INSTRENDERER_PRESENTMANAGER_H
#define INSTRENDERER_PRESENTMANAGER_H
#include "vulkan/vulkan.h"
#include "vector"
#include "Pass/RenderResource.h"
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
    uint32_t currentFrame=0;
    uint32_t swapChainImageIndex;

    std::vector<VkImage> swapchainImage;
    std::vector<VkImageView> swapchainView;
private:
    void InitSyncStructures();
};


#endif //INSTRENDERER_PRESENTMANAGER_H
