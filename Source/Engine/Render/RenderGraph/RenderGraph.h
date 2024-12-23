
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
namespace RDG
{
    bool IsImageType(ResourceType type);
    bool IsBufferType(ResourceType type);
    VkImageUsageFlags GetImageUsage(AttachmentUsage usage);
    VkBufferUsageFlagBits GetBufferUsage(ResourceType type);
    VkImageAspectFlagBits GetAspectFlag(AttachmentUsage usage);
    AttachmentState GetImageState(AttachmentUsage usage);



    class RenderGraph
    {
    public:
        RenderGraph();
        void Compile(std::shared_ptr<Scene> scene);
        void Execute();
        void DeclareResource();
        void DeclarePass();
    public:
        ResourceMap resourceMap;
        PassMap passMap;
    private:
        Handle AddResource(const ResourceRef& resource);
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
        void InsertBarrier();
        void CreateImageResource(ResourceRef& resource);
        void CreateBufferResource(ResourceRef& resource);
    private:
        std::shared_ptr<Scene> scene;
        CommandList commandList;
        HandleAllocator handleAllocator;

    };

} // RDG

#endif //INSTRENDERER_RENDERGRAPH_H
