
#ifndef INSTRENDERER_RENDERGRAPH_H
#define INSTRENDERER_RENDERGRAPH_H
#include "vulkan/vulkan.h"
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>
#include "../Buffer/Buffer.h"
#include "../Buffer/Texture.h"
namespace RDG
{
    using ResourceName = std::string;
    using PassName = std::string;
    using ShaderName = std::string;

    enum class ResourceType
    {
        Uniform,
        Texture,
        SSBO,
        SampledTexture,
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
    };

    struct PassData
    {
        VkRenderPass passHandle;
        VkFramebuffer framebufferHandle;
    };

    struct PassRef
    {
        RenderPassType type;
        std::unordered_set<ResourceName> input;
        std::unordered_set<ResourceName> output;
        PipelineRef pipeline;
        std::function<void()> executeFunc;
        std::vector<PassName> producers;
        PassData data;
    };

    struct ResourceRef
    {
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

    using ResourceMap = std::unordered_map<ResourceName,ResourceRef>;
    using PassMap = std::unordered_map<PassName,PassRef>;

    bool IsImageType(ResourceType type);
    bool IsBufferType(ResourceType type);
    VkImageUsageFlags GetImageUsage(AttachmentUsage usage);
    VkBufferUsageFlagBits GetBufferUsage(ResourceType type);
    VkImageAspectFlagBits GetAspectFlag(AttachmentUsage usage);
    AttachmentState GetImageState(AttachmentUsage usage);
    class RenderGraph
    {
    public:
        void Compile();
        void Execute();
        void DeclareResource();
        void DeclarePass();
        ResourceMap resourceMap;
        PassMap passMap;
    private:
        void CreateRenderPass();
        void CreateGraphicPass(PassRef& passData);
        void CreateComputePass(PassRef& passData);
        void CreatePresentPass(PassRef& passData);
        void WriteDependency();
        void CreateResource();
        void CreateDescriptor();
        void InsertBarrier();
        void CreateImageResource(ResourceRef& resource);
        void CreateBufferResource(ResourceRef& resource);
    };

} // RDG

#endif //INSTRENDERER_RENDERGRAPH_H
