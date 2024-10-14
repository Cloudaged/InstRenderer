
#ifndef INSTRENDERER_VULKANCONTEXT_H
#define INSTRENDERER_VULKANCONTEXT_H
#include "memory"
#include "vulkan/vulkan.h"
#include "RHI/Extensions.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

#include <stdexcept>
#include "iostream"
#include "RHI/VulkanFuncs.h"
#include "RHI/Queues.h"
#include "set"
#include "RHI/Swapchain.h"
#include "vma/vk_mem_alloc.h"

#include "Buffer/AllocatedImage.h"
#include "PresentManager.h"
#include "Buffer/Buffer.h"
#include "Buffer/BufferAllocator.h"
#include "RenderPassManager.h"
class VulkanContext
{
public:
    static VulkanContext& GetContext();
    static void Init(SDL_Window* window,SDL_Event* event);


    //Vulkan Object
    VkExtent2D windowExtent;

    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;
    VkDevice device;
    SwapchainData swapchainData;

    ContextQueues queues;

    QueueFamilyIndices familyIndices;
    VmaAllocator allocator;
    VkCommandPool cmdPool;
    PresentManager presentManager;
    VkDescriptorPool pool;
    BufferAllocator bufferAllocator;
    bool isResize = false;
    SDL_Window* sdlWindow;
    SDL_Event* sdlEvent;

    RenderPassManager* passManager;

    VkCommandBuffer drawCmd;
    void DrawPrepare();
    void Submit();

    void CreateSwapchain();

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
private:
    VulkanContext(SDL_Window* window);
    void InitVulkanBackend();
    static std::unique_ptr<VulkanContext> Pcontext;

    //Init
    void CreateInstance();
    void CreateSurface();
    void PickupPhysicalDevice();
    void CreateQueueAndDevice();
    void CreateVMAAllocator();
    void CreateDrawImage();
    void CreateCommandPool();
    void CreateDescriptorPool();
};


#endif //INSTRENDERER_VULKANCONTEXT_H
