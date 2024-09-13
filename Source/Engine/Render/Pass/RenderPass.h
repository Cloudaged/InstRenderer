
#ifndef INSTRENDERER_RENDERPASS_H
#define INSTRENDERER_RENDERPASS_H
#include "vulkan/vulkan.h"
#include "string"
#include "RenderResource.h"
#include "vector"
#include "entt.hpp"
#include "../../Common/Component/Components.h"
#include "../../Common/Scene.h"
#include "../RenderState.h"

class RenderPass
{
private:
    struct LoadStoreOP
    {
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
    };
public:

    virtual void SetupAttachments() = 0;
    virtual void Execute(entt::view<entt::get_t<Renderable>> view) = 0;
    void Build();
    VkRenderPass passHandle;
    VkFramebuffer framebufferHandle;
    int width,height;
    std::string name;
    RenderState renderState;
protected:
    VkImageUsageFlagBits GetUsage(AttachmentUsage usage);
    VkImageLayout GetLayout(AttachmentUsage usage);
    LoadStoreOP GetLSOP(AttachmentOP op);
    virtual void SetupRenderState()=0;
    std::vector<AttachmentDes> attDes;

private:
    void BuildPresentFrame();
};


#endif //INSTRENDERER_RENDERPASS_H
