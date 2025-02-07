
#ifndef INSTRENDERER_MESH_H
#define INSTRENDERER_MESH_H
#include "Buffer/Buffer.h"
#include "volk.h"
#include "vector"
#include "Vertex.h"
using Handle = uint32_t;
class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertData,const std::vector<uint32_t>& index);
    Buffer vertBuffer;
    Buffer indexBuffer;
    Buffer transformBuffer;
    size_t indexCount = 0;
    size_t vertexCount = 0;
    VkDeviceAddress vertAddress;
    VkDeviceAddress indexAddress;
    VkDeviceAddress transformAddress;
};


#endif //INSTRENDERER_RESMESH_H
