
#ifndef INSTRENDERER_RESMESH_H
#define INSTRENDERER_RESMESH_H
#include "../Render/Vertex.h"

namespace Res
{

    class ResMesh
    {
    public:
        ResMesh(std::vector<Vertex> vertices, std::vector<uint32_t> index);
        std::vector<Vertex> verts;
        std::vector<uint32_t> index;
    };

} // Res

#endif //INSTRENDERER_RESMESH_H
