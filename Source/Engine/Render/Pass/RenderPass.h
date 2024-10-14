
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

typedef std::unordered_map<std::string,AttachmentDes> AttachmentMap;

class RenderPass
{
private:
    struct LoadStoreOP
    {
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
    };
public:
    static AttachmentMap attachmentMap;

    virtual void SetupAttachments() = 0;
    void Build();
    VkRenderPass passHandle;
    VkFramebuffer framebufferHandle=VK_NULL_HANDLE;
    uint32_t width,height;
    std::string name;
    RenderState renderState;
    void ClearRes();
    void RecreatePassRes();
protected:
    VkImageUsageFlags GetUsage(AttachmentUsage usage);
    VkImageLayout GetLayout(AttachmentUsage usage);
    LoadStoreOP GetLSOP(AttachmentOP op);
    virtual void SetupRenderState()=0;
    void InputAttachmentDes(std::vector<std::string> names);

    std::vector<AttachmentDes> inputAttDes;
    std::vector<AttachmentDes> outputAttDes;

    VkDescriptorSetLayout inputAttDesLayout;
    VkDescriptorSet inputAttDesSet;

private:
    void BuildPresentFrame();
};


#endif //INSTRENDERER_RENDERPASS_H
