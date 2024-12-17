
#ifndef INSTRENDERER_RENDERGRAPH_H
#define INSTRENDERER_RENDERGRAPH_H
#include "PassRef.h"
#include "ResourceRef.h"
#include "unordered_map"
namespace RDG
{

    class RenderGraph
    {
    public:
        RenderGraph();
        void DeclareResource(std::string name);
        void AddGraphicNode(std::string name);
        void AddComputeNode();
        std::unordered_map<std::string,ResourceRef> resources;
        std::unordered_map<std::string,PassRef> passes;
    };

} // RDG

#endif //INSTRENDERER_RENDERGRAPH_H
