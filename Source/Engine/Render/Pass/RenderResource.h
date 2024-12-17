
#ifndef INSTRENDERER_RENDERRESOURCE_H
#define INSTRENDERER_RENDERRESOURCE_H
#include "vulkan/vulkan.h"
#include "string"

#include "../Buffer/AllocatedImage.h"
#include "unordered_map"
#include "../Buffer/Texture.h"
#include "memory"
#include <optional>

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

struct BufferInfo
{
    uint32_t size;
};

struct TextureInfo
{
    std::string name;
    int width,height;
    AttachmentUsage usage;
    VkFormat format;
    std::shared_ptr<Texture> data = nullptr;
    VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    bool hasInit = false;
};

struct ResourceRef
{
    ResourceType type;
    std::optional<TextureInfo> textureInfo;
    std::optional<BufferInfo> bufferInfo;
};

#endif //INSTRENDERER_RENDERRESOURCE_H
