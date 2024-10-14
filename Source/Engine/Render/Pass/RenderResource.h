
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
    ReadOnly,
    WriteOnly,
    ReadAndWrite,
    DontCare,
    Clear
};

enum class AttachmentUsage
{
    Unknown,
    ShaderReadOnly,
    ColorAttachment,
    TransferSrc,
    TransferDst,
    Present,
    Depth
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
    AttachmentOP op;
    VkFormat format;
    bool isDepthBuffer = false;
    Texture** data;
};


#endif //INSTRENDERER_RENDERRESOURCE_H
