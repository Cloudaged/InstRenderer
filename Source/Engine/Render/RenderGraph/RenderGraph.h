
#ifndef INSTRENDERER_RENDERGRAPH_H
#define INSTRENDERER_RENDERGRAPH_H
#include "../Pass/RenderPass.h"
namespace RDG
{
    using ResourceName = std::string;
    using PassName = std::string;
    using ShaderName = std::string;

    struct RenderParam
    {

    };

    struct PipelineRef
    {
        PipelineType type;
        ShaderName vertShader;
        ShaderName fragShader;
    };

    struct PassRef
    {
        std::unordered_set<ResourceName> input;
        std::unordered_set<ResourceName> output;
        PipelineRef pipeline;
        std::function<void()> executeFunc;
        std::vector<PassName> producers;
    };

    using ResourceMap = std::unordered_map<ResourceName,ResourceRef>;
    using PassMap = std::unordered_map<PassName,PassRef>;

    class RenderGraph
    {
    public:
        void Compile();
        void DeclareResource();
        void DeclarePass();
        ResourceMap resourceMap;
        PassMap passMap;
    private:
        void WriteDependency();
    };

} // RDG

#endif //INSTRENDERER_RENDERGRAPH_H
