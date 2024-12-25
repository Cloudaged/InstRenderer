
#ifndef INSTRENDERER_RESMESH_H
#define INSTRENDERER_RESMESH_H
#include "../Render/Vertex.h"
#include "string"
#include "ResMaterial.h"

namespace Res
{

    class ResMesh
    {
    public:
        ResMesh(std::vector<Vertex> vertices, std::vector<uint32_t> index,std::string name,ResMaterial* material = nullptr);
        std::vector<Vertex> verts;
        std::vector<uint32_t> index;
        std::shared_ptr<Res::ResMaterial> material;
        std::string name;
    };

} // Res

#endif //INSTRENDERER_RESMESH_H
