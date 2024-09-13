
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
    void InputGlobalDesLayout(VkDescriptorSetLayout layout);
    void CreatePerMaterialLayout(std::vector<DescriptorBindingSlot> bindings);
    void CreatePerObjLayout(std::vector<DescriptorBindingSlot> bindings);
    void CreatePipeline(PipelineType type,VkRenderPass renderPass,int attachmentCount,ShaderPath path);

    VkDescriptorSet perObjDes;
    Buffer perObjDesBuffer;

    void CreatePerObjDescriptor(size_t uniformSize);
    PipelineType type;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;


    VkDescriptorSetLayout globalLayout;
    VkDescriptorSetLayout materialLayout;
    VkDescriptorSetLayout perObjLayout;

    std::vector<VkDescriptorSetLayout> layouts;
private:
    VkShaderModule LoadShaderData(std::string path);

};


#endif //INSTRENDERER_RENDERSTATE_H
