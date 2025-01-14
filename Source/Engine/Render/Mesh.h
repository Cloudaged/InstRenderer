
#ifndef INSTRENDERER_MESH_H
#define INSTRENDERER_MESH_H
#include "Buffer/Buffer.h"
#include "vulkan/vulkan.h"
#include "vector"
#include "Vertex.h"

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertData,const std::vector<uint32_t>& index);
    Buffer vertBuffer;
    Buffer indexBuffer;
    size_t indexCount = 0;
    size_t vertexCount = 0;
    VkDeviceAddress vertAddress;
    VkDeviceAddress indexAddress;
};


#endif //INSTRENDERER_RESMESH_H
