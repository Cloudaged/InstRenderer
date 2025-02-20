
#include "ResMesh.h"

namespace Res
{
    ResMesh::ResMesh(std::vector<Vertex> vertices, std::vector<uint32_t> index,std::string name,ResMaterial* material):
    verts(vertices), index(index),name(name),material(material)
    {

    }
} // Res