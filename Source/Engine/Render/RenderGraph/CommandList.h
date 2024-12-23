
#ifndef INSTRENDERER_COMMANDLIST_H
#define INSTRENDERER_COMMANDLIST_H
#include "vulkan/vulkan.h"
#include "RenderResource.h"
#include "memory"
#include "../Buffer/Buffer.h"
namespace RDG
{

    class CommandList
    {
    public:
        void BeginCommand();
        void EndCommand();
        void BeginRenderPass(const PassRef& passRef);
        void EndRenderPass();
        void BindDescriptor();
        void BindPipeline();
        void DrawMesh(const Buffer& vertBuffer,const Buffer& indexBuffer,const size_t& indexSize);
        void DrawRenderQuad();
        void PushConstantsForHandles(void* data);
    private:
        std::shared_ptr<PassRef> curPass;
        VkCommandBuffer cmd;
    };

} // RDG

#endif //INSTRENDERER_COMMANDLIST_H
