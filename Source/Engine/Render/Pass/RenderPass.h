
#ifndef INSTRENDERER_RENDERPASS_H
#define INSTRENDERER_RENDERPASS_H
#include "vulkan/vulkan.h"
#include "string"
#include "RenderResource.h"
#include "vector"
#include "entt.hpp"
#include "../../Common/Component/Components.h"
#include "../RenderState.h"
#include "memory"
#include "../Uniforms.h"
#include "optional"



struct PassInput
{
    ResourceRef& resRef;
    AttachmentOP opt;
};

struct AttachmentState
{
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkImageLayout initLayout;
    VkImageLayout finalLayout;
};
struct GlobalDescriptorData;

class RenderPass
{
public:
    RenderPass();
    void Build();
    virtual ~RenderPass();
    virtual void ClearRes();
public:
    VkRenderPass passHandle;
    VkFramebuffer framebufferHandle=VK_NULL_HANDLE;
    uint32_t width,height;
    std::string name;
    RenderState renderState;
protected:
    VkImageUsageFlags GetUsage(AttachmentUsage usage);
    VkImageLayout GetLayout(AttachmentUsage usage);
    AttachmentState GetState(AttachmentOP op,AttachmentUsage usage);
    void TransAttachmentLayout(VkCommandBuffer cmd);
    void UpdateRecordedLayout();
    virtual void SetupRenderState()=0;
    virtual void SetupAttachments() = 0;
protected:
    std::vector<TextureInfo> inputAttDes;
    std::vector<PassInput> outputResource;
    VkDescriptorSetLayout inputAttDesLayout;
    VkDescriptorSet inputAttDesSet;
private:
    void InputAttachmentDes(std::vector<std::string> names);
    void AllocAttachmentResource(TextureInfo& attachment);
    void BuildPresentFrame();
private:
};


#endif //INSTRENDERER_RENDERPASS_H
