
#ifndef INSTRENDERER_SWAPCHAIN_H
#define INSTRENDERER_SWAPCHAIN_H
#include "vulkan/vulkan.h"
#include "vector"

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities; //number of the images in swap chain and the size of images
    std::vector<VkSurfaceFormatKHR> formats;//pixel format,color space
    std::vector<VkPresentModeKHR> presentModes;
};

struct SwapchainData
{
    VkSwapchainKHR swapchain;
    VkFormat format;
    VkExtent2D extent;
};



#endif //INSTRENDERER_SWAPCHAIN_H
