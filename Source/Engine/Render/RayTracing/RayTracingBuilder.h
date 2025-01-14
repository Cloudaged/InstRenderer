
#ifndef INSTRENDERER_RAYTRACINGBUILDER_H
#define INSTRENDERER_RAYTRACINGBUILDER_H
#include "vulkan/vulkan.h"
#include "../Mesh.h"
#include "../../Common/Component/Components.h"
#include "entt.hpp"
#include "../../Common/Scene/Scene.h"
#include <algorithm>

struct BLAS
{
    Buffer asBuffer;
    VkAccelerationStructureKHR accelerationStructure;
    VkDeviceAddress address;
    uint32_t vertexCount;
};

struct TLAS
{
    Buffer asBuffer;
    VkAccelerationStructureKHR accelerationStructure;
    VkDeviceAddress address;
};

struct RTScene
{
    TLAS topAS;
    std::vector<BLAS> allBlas;

};


class RayTracingBuilder
{
public:
    RTScene CreateRTScene(std::shared_ptr<Scene> scene);
private:
    std::vector<BLAS> CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view,VkCommandBuffer cmd);
    TLAS CreateTLAS(VkCommandBuffer cmd);
};


#endif //INSTRENDERER_RAYTRACINGBUILDER_H
