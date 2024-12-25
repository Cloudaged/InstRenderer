
#ifndef INSTRENDERER_COMMANDLIST_H
#define INSTRENDERER_COMMANDLIST_H
#include "vulkan/vulkan.h"
#include "RenderResource.h"
#include "memory"
#include "../Buffer/Buffer.h"
#include "../Mesh.h"
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
        void DrawMesh(const Mesh& mesh);
        void DrawRenderQuad();
        void PushConstantsForHandles(void* data);
    public:
        VkCommandBuffer cmd;
    private:
        std::shared_ptr<PassRef> curPass;
    };

} // RDG

#endif //INSTRENDERER_COMMANDLIST_H
