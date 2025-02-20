
#ifndef INSTRENDERER_RESNODE_H
#define INSTRENDERER_RESNODE_H
#include <vector>
#include "ResMesh.h"
#include "memory"
namespace Res
{
    class ResNode
    {
    public:
        ResNode(std::shared_ptr<ResNode>,std::string name);
        std::string name;
        std::shared_ptr<ResNode> parent;
        std::vector<std::shared_ptr<ResNode>> children;
        std::vector<std::shared_ptr<Res::ResMesh>> meshes;
    };

}

#endif //INSTRENDERER_RESNODE_H
