
#ifndef INSTRENDERER_SKYBOX_H
#define INSTRENDERER_SKYBOX_H
#include "Buffer.h"
#include "string"
#include "vector"
#include "iostream"
#include "../../Resource/ImageLoader.h"
#include "../../Resource/ModelLoader.h"
class Skybox
{
public:
    Skybox(std::string boxPath,std::vector<std::string> paths);
    Buffer boxVertBuffer;
    Buffer boxIndexBuffer;
    VkImage vk_image;
    VkImageView imageView;
    VkSampler sampler;
    VkImageLayout layout;
    VkFormat format;
    VkImageUsageFlags usage;

    int indicesCount;
private:
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;

};


#endif //INSTRENDERER_SKYBOX_H
