
#ifndef INSTRENDERER_EXTENSIONS_H
#define INSTRENDERER_EXTENSIONS_H
#include "vulkan/vulkan.h"
#include "vector"

const std::vector<const char*> validationLayers =
        {
                "VK_LAYER_KHRONOS_validation"
        };
const std::vector<const char*> deviceExtensions =
        {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                "VK_KHR_buffer_device_address",
                "VK_KHR_synchronization2",
                VK_KHR_MULTIVIEW_EXTENSION_NAME
        };


#endif //INSTRENDERER_EXTENSIONS_H
