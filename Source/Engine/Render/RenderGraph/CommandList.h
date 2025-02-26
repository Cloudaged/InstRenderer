
#ifndef INSTRENDERER_COMMANDLIST_H
#define INSTRENDERER_COMMANDLIST_H
#include "volk.h"
#include "RenderResource.h"
#include "memory"
#include "../Buffer/Buffer.h"
#include "../Mesh.h"
#include "../TextureExtent.h"
namespace RDG
{

    class CommandList
    {
    public:
        void BeginCommand();
        void AddBarrier(TextureInfo& texture,VkImageLayout dstLayout);
        void EndCommand();
        bool BeginRenderPass(const PassRef& passRef);
        void EndRenderPass();
        void BindDescriptor();
        void BindPipeline();
        void DrawMesh(const Mesh& mesh);
        void DrawRenderQuad();
        void PushConstantsForHandles(void* data);
        void RayTracing();
        void Dispatch(int groupX, int groupY,int groupZ);
        void DrawMeshTask(int groupCount);
        void DrawInstances(const Mesh& mesh,int instanceCount);
        void TransImage(TextureInfo& src,TextureInfo& dst,VkImageLayout srcFinalLayout,VkImageLayout dstFinalLayout);
    public:
        VkCommandBuffer cmd;
    private:
        std::shared_ptr<PassRef> curPass;
    };

} // RDG

#endif //INSTRENDERER_COMMANDLIST_H
