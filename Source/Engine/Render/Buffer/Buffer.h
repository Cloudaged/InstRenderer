
#ifndef INSTRENDERER_BUFFER_H
#define INSTRENDERER_BUFFER_H
#include "volk.h"
#include "vma/vk_mem_alloc.h"
class Buffer
{
public:
    VkBuffer vk_buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};


#endif //INSTRENDERER_BUFFER_H
