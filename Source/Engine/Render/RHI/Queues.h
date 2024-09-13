
#ifndef INSTRENDERER_QUEUES_H
#define INSTRENDERER_QUEUES_H
#include "optional"
#include "vulkan/vulkan.h"


struct QueueFamilyIndices
{
    //QueueFamilies that we need
    std::optional<uint32_t> graphicFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete()
    {
        return graphicFamily.has_value()&&presentFamily.has_value();
    }
};

struct ContextQueues
{
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

#endif //INSTRENDERER_QUEUES_H
