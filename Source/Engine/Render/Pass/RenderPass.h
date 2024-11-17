
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


struct RenderResource
{
    AttachmentDes& attDes;
    AttachmentOP opt;
};

struct AttachmentState
{
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkImageLayout initLayout;
    VkImageLayout finalLayout;
};

class RenderPass
{
private:

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
    void TransAttachmentLayout(VkCommandBuffer cmd);
    void UpdateRecordedLayout();
    VkImageUsageFlags GetUsage(AttachmentUsage usage);
    VkImageLayout GetLayout(AttachmentUsage usage);
    AttachmentState GetState(AttachmentOP op,AttachmentUsage usage);
    virtual void SetupRenderState()=0;
    void InputAttachmentDes(std::vector<std::string> names);

    std::vector<AttachmentDes> inputAttDes;
    std::vector<RenderResource> outputResource;

    VkDescriptorSetLayout inputAttDesLayout;
    VkDescriptorSet inputAttDesSet;

private:
    void BuildPresentFrame();
};


#endif //INSTRENDERER_RENDERPASS_H
