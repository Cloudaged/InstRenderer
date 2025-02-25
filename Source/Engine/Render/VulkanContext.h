
#ifndef INSTRENDERER_VULKANCONTEXT_H
#define INSTRENDERER_VULKANCONTEXT_H
#include "memory"
#include "RHI/Extensions.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "volk.h"
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
#include "Uniforms.h"


enum class CmdThread
{
    Game=0,
    Resource=1,
    Editor=2
};
class VulkanContext
{
public:
    static VulkanContext& GetContext();
    static void Init(SDL_Window* window,SDL_Event* event);
    void DrawPrepare();
    void Submit();
    void CreateSwapchain();
    VkCommandBuffer BeginSingleTimeCommands(CmdThread thread);
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer,CmdThread thread);
public:
    VkExtent2D windowExtent;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;
    VkDevice device;
    SwapchainData swapchainData;
    ContextQueues queues;
    QueueFamilyIndices familyIndices;
    VmaAllocator allocator;
    VkCommandPool gameCmdPool;
    VkCommandPool computeCmdPool;
    VkCommandPool resourceCmdPool;
    VkCommandPool editorCmdPool;
    PresentManager presentManager;
    VkDescriptorPool pool;
    BufferAllocator bufferAllocator;
    bool isResize = false;
    SDL_Window* sdlWindow;
    SDL_Event* sdlEvent;
    //RenderPassManager* passManager;
    VkCommandBuffer drawCmd;
    bool isFirstFrame = true;
    VkDescriptorSetLayout bindlessLayout;
    VkDescriptorSet bindlessSet;
private:
    VulkanContext(SDL_Window* window);
    void InitVulkanBackend();
    //Init
    VkResult SetupDebugMessenger();
    void SetupExtensionFunc();
    void CreateInstance();
    void CreateSurface();
    void PickupPhysicalDevice();
    void CreateQueueAndDevice();
    void CreateVMAAllocator();
    void CreateDrawImage();
    void CreateCommandPool();
    void CreateDescriptorPool();
    void CreateTestGlobalDescriptorSetLayout();
    //void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
private:
    static std::unique_ptr<VulkanContext> Pcontext;
    VkDebugUtilsMessengerEXT debugMessenger;
    bool isBindlessSupported = false;
};


#endif //INSTRENDERER_VULKANCONTEXT_H
