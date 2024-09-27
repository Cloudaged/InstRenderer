
#ifndef INSTRENDERER_RENDERRESOURCE_H
#define INSTRENDERER_RENDERRESOURCE_H
#include "vulkan/vulkan.h"
#include "string"

#include "../Buffer/AllocatedImage.h"
#include "unordered_map"



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
    DontCare
};

enum class AttachmentUsage
{
    Unknown,
    ShaderReadOnly,
    ColorAttachment,
    TransferSrc,
    TransferDst,
    Present
};

struct AttachmentData
{
    VkImageView view;
    VkImage image;
};
struct AttachmentDes
{
    int width,height;
    AttachmentUsage usage;
    AttachmentOP op;
    VkFormat format;
    bool isDepthBuffer = false;
    AllocatedImage** data;
};

typedef std::unordered_map<std::string,AttachmentDes> AttachmentMap;

static AttachmentMap attachmentMap;

#endif //INSTRENDERER_RENDERRESOURCE_H
