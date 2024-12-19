
#ifndef INSTRENDERER_RENDERGRAPH_H
#define INSTRENDERER_RENDERGRAPH_H
#include "../Pass/RenderPass.h"
namespace RDG
{
    using ResourceName = std::string;
    using PassName = std::string;
    using ShaderName = std::string;

    struct RenderParam
    {

    };

    enum class RenderPassType
    {
        Graphic,
        Compute,
        Present
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
