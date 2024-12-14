
#ifndef INSTRENDERER_RESNODE_H
#define INSTRENDERER_RESNODE_H
#include <vector>
#include "ResMesh.h"
namespace Res
{
    class ResNode
    {
    public:
        ResNode(ResNode* parent,std::string name);
        std::string name;
        ResNode* parent;
        std::vector<ResNode*> children;
        std::vector<Res::ResMesh*> meshes;
    };

}

#endif //INSTRENDERER_RESNODE_H
