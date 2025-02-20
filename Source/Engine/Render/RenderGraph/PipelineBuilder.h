
#ifndef INSTRENDERER_PIPELINEBUILDER_H
#define INSTRENDERER_PIPELINEBUILDER_H
#include "RenderResource.h"
#include "fstream"
#include "../../Common/Core/PathDefine.h"
#include "../Vertex.h"
#include "../RayTracing/RTBuilder.h"
namespace RDG
{
    struct PipelineSettings
    {
        bool depthTest;
        bool depthWrite;
        bool hasVertAtt;
        VkCullModeFlags cullMode;
    };


    class PipelineBuilder
    {
    public:
        static VkShaderModule LoadShaderData(std::string path);
        static void CreatePipeline(PipelineRef& pipelineRef,int attCount,VkRenderPass renderPass);
    private:
        static void CreateRayTracingPipeline(PipelineRef& pipelineRef);
        static void CreateComputePipeline(PipelineRef& pipelineRef);
        static std::vector<char> ReadFile(const std::string &fileName);
        static std::string GetShaderFullPath(std::string name);
    };
}


#endif //INSTRENDERER_PIPELINEBUILDER_H
