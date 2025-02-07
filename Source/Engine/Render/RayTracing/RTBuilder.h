
#ifndef INSTRENDERER_RTBUILDER_H
#define INSTRENDERER_RTBUILDER_H
#include "volk.h"
#include "../Mesh.h"
#include "../../Common/Component/Components.h"
#include "entt.hpp"
#include <algorithm>
#include "../Uniforms.h"
struct BLAS
{
    Buffer* asBuffer;
    VkAccelerationStructureKHR accelerationStructure;
    VkDeviceAddress address;
    //GeometryNode geometryNode;
    Handle nodeHandle;
};

struct TLAS
{
    Buffer* asBuffer;
    VkAccelerationStructureKHR accelerationStructure;
    VkDeviceAddress tlasAddress;
    void* instancesHostAddress;
    VkDeviceAddress instancesAddress;
    VkDeviceAddress scratchAddress;
    std::vector<VkAccelerationStructureInstanceKHR> instances;
};

struct RTScene
{
    TLAS topAS;
    std::vector<BLAS> allBlas;
    bool isEmpty = true;
};

struct SBT
{
    Buffer* genBuffer;
    Buffer* missBuffer;
    Buffer* hitBuffer;
    VkDeviceAddress deviceAddress;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups;
    VkStridedDeviceAddressRegionKHR genRegion;
    VkStridedDeviceAddressRegionKHR missRegion;
    VkStridedDeviceAddressRegionKHR hitRegion;
    VkStridedDeviceAddressRegionKHR callRegin;
    uint32_t shaderHandleSize;

};

//enum class RTStageIndices : uint32_t
//{
//    Gen = 0,
//    Miss = 1,
//    CloestHit = 2,
//    ShaderGroupCount = 3
//};


class RTBuilder
{
public:
    static RTScene CreateRTScene(entt::view<entt::get_t<Renderable,Transform>> view);
    static void UpdateTransform(TLAS& tlas);
private:
    static BLAS CreateEmptyBLAS();
    static std::vector<BLAS> CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view);
    static TLAS CreateTLAS(const std::vector<BLAS>& allblas);
};


#endif //INSTRENDERER_RTBUILDER_H
