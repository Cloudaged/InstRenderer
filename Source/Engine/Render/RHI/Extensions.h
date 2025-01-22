
#ifndef INSTRENDERER_EXTENSIONS_H
#define INSTRENDERER_EXTENSIONS_H
#include "volk.h"
#include "vector"

const std::vector<const char*> validationLayers =
        {
                "VK_LAYER_KHRONOS_validation"
        };
const std::vector<const char*> deviceExtensions =
        {
                VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                "VK_KHR_buffer_device_address",
                "VK_KHR_synchronization2",
                VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                VK_KHR_MULTIVIEW_EXTENSION_NAME,
                VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                VK_KHR_SPIRV_1_4_EXTENSION_NAME,
                VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME
        };
\

#endif //INSTRENDERER_EXTENSIONS_H
