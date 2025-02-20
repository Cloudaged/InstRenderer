
#ifndef INSTRENDERER_VULKANFUNCS_H
#define INSTRENDERER_VULKANFUNCS_H

#include "volk.h"
#include "vector"
#include "Queues.h"
#include "Swapchain.h"
#include "limits"
#include "functional"

class VulkanFuncs
{
public:
    static bool IsDeviceSuitable(VkPhysicalDevice device);


    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats,VkFormat format,VkColorSpaceKHR colorSpace);
    static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresnetModes,VkPresentModeKHR mode);
    static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

};


#endif //INSTRENDERER_VULKANFUNCS_H
