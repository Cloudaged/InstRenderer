
#ifndef INSTRENDERER_BUFFERALLOCATOR_H
#define INSTRENDERER_BUFFERALLOCATOR_H
#include "Buffer.h"
#include "Texture.h"
class BufferAllocator
{
public:
    Buffer* CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    AllocatedImage* CreateImageBuffer(VkExtent2D extent2D, VkFormat format, VkImageUsageFlags usage);
    void LoadTextureToImageBuffer();
    void* GetMappedMemory(Buffer allocatedBuffer);
    void DestroyBuffer(Buffer buffer);

    void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout,int mipLevels=1);
};


#endif //INSTRENDERER_BUFFERALLOCATOR_H
