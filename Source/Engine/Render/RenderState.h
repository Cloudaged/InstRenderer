
#ifndef INSTRENDERER_RENDERSTATE_H
#define INSTRENDERER_RENDERSTATE_H
#include "vulkan/vulkan.h"
#include "vector"
#include "fstream"
#include "Buffer/BufferAllocator.h"
#include "Vertex.h"
enum class PipelineType
{
    Mesh,
    Skybox,
    RenderQuad
};

struct PipelineSettings
{
    bool depthTest;
    bool depthWrite;
    VkCullModeFlags cullMode;

};

struct DescriptorBindingSlot
{
    uint32_t bindingPos;
    VkShaderStageFlagBits flags;
    VkDescriptorType type;
};

struct ShaderPath
{
    std::string vertPath;
    std::string fragPath;
};




class RenderState
{
public:
    RenderState();
    void CreatePipeline(PipelineType type,VkRenderPass renderPass,int attachmentCount,ShaderPath path);

    PipelineType type;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<VkDescriptorSetLayout> layouts;
    bool isInit = false;
private:
    VkShaderModule LoadShaderData(std::string path);

};


#endif //INSTRENDERER_RENDERSTATE_H
