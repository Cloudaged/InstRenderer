
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


    enum class ResourceType : uint32_t
    {
        Uniform = 0,
        SSBO = 1,
        Texture = 2
    };

    enum class PipelineType
    {
        Mesh,
        Skybox,
        RenderQuad
    };

    enum class RenderPassType
    {
        Graphic,
        Compute,
        Present
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

    struct RenderParam
    {

    };

    struct BufferInfo
    {
        uint32_t size;
        void* mappedAddress = nullptr;
        std::shared_ptr<Buffer> data = nullptr;
    };

    struct TextureInfo
    {
        VkExtent2D extent;
        AttachmentUsage usage;
        VkFormat format;
        std::shared_ptr<Texture> data = nullptr;
        VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        bool hasInit = false;
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

    struct PassRef
    {
        RenderPassType type;
        uint32_t fbWidth,fbHeight;
        std::unordered_set<Handle> input;
        std::unordered_set<Handle> output;
        PipelineRef pipeline;
        std::function<void(CommandList& cmd)> executeFunc;
        std::vector<PassName> producers;
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


    struct AttachmentState
    {
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
        VkImageLayout initLayout;
        VkImageLayout finalLayout;
    };

    using ResourceMap = std::unordered_map<Handle,ResourceRef>;
    using PassMap = std::unordered_map<PassName,PassRef>;
}


#endif //INSTRENDERER_RENDERRESOURCE_H
