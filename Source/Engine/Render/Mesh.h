
#ifndef INSTRENDERER_MESH_H
#define INSTRENDERER_MESH_H
#include "Buffer/Buffer.h"
#include "vulkan/vulkan.h"
#include "vector"
#include "Vertex.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertData, std::vector<uint32_t> index);
    Buffer vertBuffer;
    Buffer indexBuffer;

    VkDeviceAddress vertAddress;
    VkDeviceAddress indexAddress;

    std::vector<Vertex> vertData;
    std::vector<uint32_t> index;
};


#endif //INSTRENDERER_RESMESH_H
