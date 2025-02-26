
#ifndef INSTRENDERER_RENDERGRAPH_H
#define INSTRENDERER_RENDERGRAPH_H
#include "volk.h"
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>
#include "../Buffer/Buffer.h"
#include "../Buffer/Texture.h"
#include "../Uniforms.h"
#include "../../Common/Core/HandleAllocator.h"
#include "RenderResource.h"
#include "PassRequires.h"
#include "CommandList.h"
#include "../../Common/Scene/Scene.h"
class RenderSystem;
namespace RDG
{
#define UBO_BINDING 0
#define SSBO_BINDING 1
#define TEXTURE_BINDING 2
#define STORE_IMAGE_BINDING 3
#define ACCELERATION_ST_BINDING 4

    bool IsImageType(ResourceType type);
    bool IsBufferType(ResourceType type);
    bool IsDepthType(UsageFlags usage);
    VkImageUsageFlags GetImageUsage(UsageFlags usage);
    VkBufferUsageFlagBits GetBufferUsage(ResourceType type);
    VkImageAspectFlagBits GetAspectFlag(UsageFlags usage);
    AttachmentState GetImageState(UsageFlags usage, bool isRW);

    class RenderGraph
    {
        friend RenderSystem;
    public:
        RenderGraph();
        void DeclareResource(std::shared_ptr<Scene> scene);
        void Compile();
        void Execute();
        Handle AddResource(ResourceRef&& resource);
        Handle AddOuterResource(ResourceRef&& resource);
        Handle GetResourceHandle(std::string name);
    public:
        ResourceMap resourceMap;
        std::shared_ptr<ResourceRef> accelerationStructure;
    private:
        void AddPass(const PassRef& pass);
        void CreateRenderPass();
        void CreateGraphicPass(PassRef& passData);
        void CreateComputePass(PassRef& passData);
        void CreatePresentPass(PassRef& passData);
        void CreateRayTracingPass(PassRef& passData);
        void WriteImageDescriptor(const ResourceRef& resource);
        void WriteBufferDescriptor(const ResourceRef& resource);
        void WriteAccelerationSTDescriptor(const ResourceRef& resource);
        void WriteDependency();
        void CreateResource();
        void CreateDescriptor();
        void CreateVkPipeline();
        void InsertBarrier(CommandList& cmd,const PassRef& passRef);
        void CreateImageResource(ResourceRef& resource);
        void CreateBufferResource(ResourceRef& resource);
        void RecreateAllPass();
        void RecreatePassResource(PassRef& pass);
        void TransitionLayout(CommandList& cmd,const PassRef& passRef);
    private:
        PassMap passArr;
        std::shared_ptr<Scene> scene;
        CommandList commandList;
        HandleAllocator handleAllocator;
        entt::view<entt::get_t<Renderable,Transform>> view;
    };

} // RDG

#endif //INSTRENDERER_RENDERGRAPH_H
