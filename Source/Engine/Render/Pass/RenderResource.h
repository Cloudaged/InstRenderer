
#ifndef INSTRENDERER_RENDERRESOURCE_H
#define INSTRENDERER_RENDERRESOURCE_H
#include "vulkan/vulkan.h"
#include "string"

#include "../Buffer/AllocatedImage.h"
#include "unordered_map"
#include "../Buffer/Texture.h"


enum class ResourceType
{
    Uniform,
    Attachment,
    SampledTexture,
};

enum class AttachmentOP
{
    WriteOnly,
    ReadAndWrite,
    Depth,
    Present,
    Clear
};

enum class AttachmentUsage
{
    Unknown,
    Color,
    TransferSrc,
    TransferDst,
    Present,
    Depth,
    Prefiltered,
    ShadowMap
};

struct AttachmentData
{
    VkImageView view;
    VkImage image;
};
struct AttachmentDes
{
    std::string name;
    int width,height;
    AttachmentUsage usage;
    VkFormat format;
    Texture** data;
    VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    bool hasInit = false;
};


#endif //INSTRENDERER_RENDERRESOURCE_H
