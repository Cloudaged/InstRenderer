
#ifndef INSTRENDERER_RENDERRESOURCE_H
#define INSTRENDERER_RENDERRESOURCE_H
#include "volk.h"
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
#include "../RayTracing/RTBuilder.h"
#include "../../Common/Core/Flags.h"
#include "TextureUsage.h"
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
        Texture = 2,
        StorageImage = 3,
        Accleration = 4
    };

    enum class PipelineType
    {
        Mesh,
        Skybox,
        RenderQuad,
        RayTracing,
        Compute
    };

    enum class RenderPassType
    {
        Raster,
        Compute,
        Present,
        RayTracing
    };

//    enum class TextureUsage :uint32_t
//    {
//        Unknown= 1<<0,
//        Color= 1<<1,
//        TransferSrc=1<<2,
//        TransferDst = 1<<3,
//        Present= 1<<4,
//        Depth= 1<<5,
//        Prefiltered= 1<<6,
//        ShadowMap= 1<<7,
//        MaterialTexture= 1<<8,
//    };

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
        UsageFlags usage;
        VkFormat format;
        std::shared_ptr<Texture> data = nullptr;
        int mipLevels = 1;
        int arrayCount = 1;
        VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    struct RasterShaders
    {
        ShaderName vert;
        ShaderName frag;
    };

    struct ComputeShaders
    {
        ShaderName comp;
    };

    struct RayTracingShaders
    {
        ShaderName chit;
        ShaderName gen;
        ShaderName miss;
        ShaderName ahit;
    };

    struct PipelineRef
    {
        PipelineType type;
        RasterShaders rsShaders;
        ComputeShaders cpShaders;
        RayTracingShaders rtShaders;
        SBT sbt;
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
        std::shared_ptr<RTScene> rtScene;
        PassName producerPass;
    };

    using ResourceMap = std::unordered_map<Handle,ResourceRef>;
    using PassMap = std::vector<PassRef>;
}


#endif //INSTRENDERER_RENDERRESOURCE_H
