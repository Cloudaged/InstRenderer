
#ifndef INSTRENDERER_RTBUILDER_H
#define INSTRENDERER_RTBUILDER_H
#include "volk.h"
#include "../Mesh.h"
#include "../../Common/Component/Components.h"
#include "entt.hpp"
#include <algorithm>

struct BLAS
{
    Buffer* asBuffer;
    VkAccelerationStructureKHR accelerationStructure;
    VkDeviceAddress address;
};

struct TLAS
{
    Buffer* asBuffer;
    VkAccelerationStructureKHR accelerationStructure;
    VkDeviceAddress address;
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
private:
    static BLAS CreateEmptyBLAS();
    static std::vector<BLAS> CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view);
    static TLAS CreateTLAS(const std::vector<BLAS>& allblas);
};


#endif //INSTRENDERER_RTBUILDER_H
