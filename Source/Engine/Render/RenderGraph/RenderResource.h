
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
#include <unordered_set>
#include <functional>
#include "../TextureExtent.h"
namespace RDG
{
    using ResourceName = std::string;
    using PassName = std::string;
    using ShaderName = std::string;
    using Handle = uint32_t;
    using DescriptorHandle = uint32_t;
    using ResHandle = std::pair<Handle,DescriptorHandle>;
    class CommandList;
    struct ShaderPath
    {
        std::string vertPath;
        std::string fragPath;
    };

    enum class ResourceAccess
    {
        ReadOnly,
        ReadWrite
    };

    enum class ResourceType : uint32_t
    {
        Uniform = 0,
        SSBO = 1,
        Attachment = 2,
        MaterialTexture = 3,
    };

    enum class PipelineType
    {
        Mesh,
        Skybox,
        RenderQuad,
        RayTracing
    };

    enum class RenderPassType
    {
        Raster,
        Compute,
        Present,
        RayTracing
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
        ShadowMap,
        MaterialTexture
    };

    struct AttachmentState
    {
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
        VkImageLayout initLayout;
        VkImageLayout finalLayout;
    };

    struct BufferInfo
    {
        uint32_t size;
        void* mappedAddress = nullptr;
        std::shared_ptr<Buffer> data = nullptr;
    };

    struct TextureInfo
    {
        TextureExtent extent;
        AttachmentUsage usage;
        VkFormat format;
        std::shared_ptr<Texture> data = nullptr;
        int mipLevels = 1;
        int arrayCount = 1;
        VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    struct PipelineRef
    {
        PipelineType type;
        ShaderName vertShader;
        ShaderName fragShader;
        size_t handleSize;
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };

    struct MaterialHandles
    {
        Handle baseColor = 0;
        Handle normal = 0;
        Handle metallicRoughness = 0;
    };

    struct PassData
    {
        VkRenderPass passHandle;
        VkFramebuffer framebufferHandle;
    };

    struct AttachmentDes
    {
        TextureInfo& texInfo;
        AttachmentState state;
    };

    struct PassRef
    {
        PassName name;
        RenderPassType type;
        TextureExtent fbExtent;
        std::vector<Handle> input;
        std::vector<Handle> output;
        PipelineRef pipeline;
        std::function<void(CommandList& cmd)> executeFunc;
        std::vector<PassName> producers;
        std::vector<AttachmentDes> attDes;
        PassData data;
        bool hasDepth = false;
    };

    struct ResourceRef
    {
        ResourceName name;
        Handle handle = 0;
        ResourceType type;
        std::optional<TextureInfo> textureInfo;
        std::optional<BufferInfo> bufferInfo;
        PassName producerPass;
    };

    using ResourceMap = std::unordered_map<Handle,ResourceRef>;
    using PassMap = std::vector<PassRef>;
}


#endif //INSTRENDERER_RENDERRESOURCE_H
