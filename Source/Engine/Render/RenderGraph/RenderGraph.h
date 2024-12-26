
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

    bool IsImageType(ResourceType type);
    bool IsBufferType(ResourceType type);
    VkImageUsageFlags GetImageUsage(AttachmentUsage usage);
    VkBufferUsageFlagBits GetBufferUsage(ResourceType type);
    VkImageAspectFlagBits GetAspectFlag(AttachmentUsage usage);
    AttachmentState GetImageState(AttachmentUsage usage);

    class RenderGraph
    {
        friend RenderSystem;
    public:
        RenderGraph();
        void Compile(std::shared_ptr<Scene> scene);
        void Execute();
        Handle AddResource(const ResourceRef& resource);
        Handle AddOuterResource(const ResourceRef& resource);
        Handle GetResourceHandle(std::string name);
    public:

    private:
        void AddPass(const PassRef& pass);
        void DeclareResource();
        void CreateRenderPass();
        void CreateGraphicPass(PassRef& passData);
        void CreateComputePass(PassRef& passData);
        void CreatePresentPass(PassRef& passData);
        void WriteImageDescriptor(const ResourceRef& resource);
        void WriteBufferDescriptor(const ResourceRef& resource);
        void WriteDependency();
        void CreateResource();
        void CreateDescriptor();
        void CreateVkPipeline();
        void InsertBarrier(const CommandList& cmd,const PassRef& passRef);
        void CreateImageResource(ResourceRef& resource);
        void CreateBufferResource(ResourceRef& resource);
        void RecreateAllPass();
        void RecreatePass(PassRef& pass);
        void ClearImageResource(ResourceRef& resource);
    private:
        ResourceMap resourceMap;
        PassMap passArr;
        std::shared_ptr<Scene> scene;
        CommandList commandList;
        HandleAllocator handleAllocator;
        entt::view<entt::get_t<Renderable,Transform>> view;
    };

} // RDG

#endif //INSTRENDERER_RENDERGRAPH_H
