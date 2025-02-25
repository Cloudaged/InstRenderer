
#include "VulkanFuncs.h"
#include "../VulkanContext.h"


bool VulkanFuncs::IsDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device,&deviceProperties);
    vkGetPhysicalDeviceFeatures(device,&deviceFeatures);

    return deviceProperties.deviceType==VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

QueueFamilyIndices VulkanFuncs::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount, queueFamilies.data());

    int i =0;
    for (const auto& queueFamily:queueFamilies)
    {
        if(queueFamily.queueFlags&VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicFamily = i;
        } else
        {
            if(queueFamily.queueFlags&VK_QUEUE_COMPUTE_BIT)
            {
                indices.computeFamily = i;
            }
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,VulkanContext::GetContext().surface,&presentSupport);
        if(presentSupport)
        {
            indices.presentFamily = i;
        }
        i++;
    }
    return indices;

}

SwapChainSupportDetails VulkanFuncs::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,VulkanContext::GetContext().surface,&details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,VulkanContext::GetContext().surface,&formatCount, nullptr);

    if(formatCount!=0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,VulkanContext::GetContext().surface,&formatCount,details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,VulkanContext::GetContext().surface,&presentModeCount, nullptr);

    if(presentModeCount !=0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,VulkanContext::GetContext().surface,&presentModeCount,details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR
VulkanFuncs::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats, VkFormat format,
                                     VkColorSpaceKHR colorSpace)
{
    for (const auto& availableFormat:availableFormats)
    {
        if(availableFormat.format==format&&availableFormat.colorSpace == colorSpace)
        {
            return availableFormat;
        }
    }
    std::cout<<"[Swapchain]Did't find suitable format\n";
    return availableFormats[0];
}

VkPresentModeKHR
VulkanFuncs::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresnetModes, VkPresentModeKHR mode)
{
    for(const auto& availablePresent: availablePresnetModes)
    {
        if(availablePresent ==mode)
        {
            return availablePresent;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanFuncs::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if(capabilities.currentExtent.width!=std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    } else
    {
        VkExtent2D actualExtent = VulkanContext::GetContext().windowExtent;
        actualExtent.height = std::clamp(actualExtent.height,capabilities.minImageExtent.height,capabilities.maxImageExtent.height);
        actualExtent.width = std::clamp(actualExtent.width,capabilities.minImageExtent.width,capabilities.maxImageExtent.width);
        return actualExtent;
    }
}



