
#include "RenderGraph.h"
#include "../VulkanContext.h"

RDG::RenderGraph renderGraph;
namespace RDG
{
    void RenderGraph::DeclareResource()
    {

        int winWidth = VulkanContext::GetContext().windowExtent.width;
        int winHeight = VulkanContext::GetContext().windowExtent.height;

        resourceMap["Position"] = {.type = ResourceType::Attachment,
                .textureInfo =TextureInfo{"Position", winWidth, winHeight,
                                          AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["Normal"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{"Normal", winWidth, winHeight,
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["BaseColor"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{"BaseColor", winWidth, winHeight,
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["MetallicRoughness"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{"MetallicRoughness", winWidth, winHeight,
                                           AttachmentUsage::Color, VK_FORMAT_R16G16B16A16_SFLOAT}};

        resourceMap["Depth"] = {.type = ResourceType::Attachment,
                .textureInfo = TextureInfo{"Depth", winWidth, winHeight,
                                           AttachmentUsage::Depth, VK_FORMAT_D32_SFLOAT}};
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