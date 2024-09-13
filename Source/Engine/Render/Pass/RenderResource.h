
#ifndef INSTRENDERER_RENDERRESOURCE_H
#define INSTRENDERER_RENDERRESOURCE_H
#include "vulkan/vulkan.h"
#include "string"
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
    std::string name;
    AttachmentUsage usage;
    AttachmentOP op;
    VkFormat format;
    bool isDepthBuffer = false;
    AttachmentData* data;
};



#endif //INSTRENDERER_RENDERRESOURCE_H
