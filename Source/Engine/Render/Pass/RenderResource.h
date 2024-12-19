
#ifndef INSTRENDERER_RENDERRESOURCE_H
#define INSTRENDERER_RENDERRESOURCE_H
#include "vulkan/vulkan.h"
#include "string"

#include "../Buffer/AllocatedImage.h"
#include "unordered_map"
#include "../Buffer/Texture.h"
#include "../Buffer/Buffer.h"
#include "memory"
#include <optional>

enum class ResourceType
{
    Uniform,
    Texture,
    SSBO,
    SampledTexture,
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


#endif //INSTRENDERER_RENDERRESOURCE_H
