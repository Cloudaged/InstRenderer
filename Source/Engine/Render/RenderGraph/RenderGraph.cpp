
#include "RenderGraph.h"
#include "../VulkanContext.h"

RDG::RenderGraph renderGraph;
namespace RDG
{
    void RenderGraph::DeclareResource()
    {
        int shadowMapWidth = 2048;
        int shadowMapHeight = 2048;

        int winWidth = VulkanContext::GetContext().windowExtent.width;
        int winHeight = VulkanContext::GetContext().windowExtent.height;

        resourceMap["Position"] = {.type = ResourceType::Attachment,
                .textureInfo =TextureInfo{winWidth, winHeight,
                                          AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["Normal"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{ winWidth, winHeight,
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["BaseColor"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{winWidth, winHeight,
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["MetallicRoughness"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{ winWidth, winHeight,
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["Depth"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{winWidth, winHeight,
                                           AttachmentUsage::Depth, VK_FORMAT_D32_SFLOAT}};

        resourceMap["ShadowMap"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{winWidth, winHeight,
                                           AttachmentUsage::ShadowMap, VK_FORMAT_D32_SFLOAT}};

        resourceMap["Lighted"] = {.type = ResourceType::Attachment,
                                .textureInfo = TextureInfo{ winWidth, winHeight,
                                AttachmentUsage::Color, VK_FORMAT_R8G8B8A8_SRGB}};

        resourceMap["Present"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{ winWidth, winHeight,
                                            AttachmentUsage::Present, VK_FORMAT_B8G8R8A8_SRGB}};

    }

    void RenderGraph::DeclarePass()
    {
        passMap["Geometry"] = {.input = {},
                               .output = {"Position","Normal","BaseColor","MetallicRoughness","Depth"},
                               .pipeline = {PipelineType::Mesh,"GeometryVert.spv","GeometryFrag.spv"},
                               .executeFunc = [&]()
                               {
                                    int b = 6;
                               }};

        passMap["Shadow"] =    {.input = {"Position"},
                                .output = {"ShadowMap"},
                                .pipeline = {PipelineType::Mesh,"ShadowMapVert.spv","ShadowMapFrag.spv"},
                                .executeFunc = [&]()
                                {
                                    int b = 6;
                                }};

        passMap["Composition"] = {.input = {"Position","Normal","BaseColor","MetallicRoughness"},
                                .output = {"Lighted"},
                                .pipeline = {PipelineType::RenderQuad,"CompositionVert.spv","CompositionFrag.spv"},
                                .executeFunc = [&]()
                                {
                                    int b = 6;
                                }};
    }

    void RenderGraph::Compile()
    {
        WriteDependency();

    }

    void RenderGraph::WriteDependency()
    {
        //Write Resources' producer
        for (auto& [passName,passData] :passMap)
        {
            for (auto& resName : passData.output)
            {
                resourceMap[resName].producerPass = passName;
            }
        }
        //Write Passes' producer
        for (auto& [passName,passData] : passMap)
        {
            for (auto& resName : passData.input)
            {
                passData.producers.push_back(resourceMap[resName].producerPass);
            }
        }
    }
} // RDG