
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
    uint32_t vertexCount;
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

};


class RTBuilder
{
public:
    static RTScene CreateRTScene(entt::view<entt::get_t<Renderable,Transform>> view);
private:
    static std::vector<BLAS> CreateBLAS(entt::view<entt::get_t<Renderable,Transform>> view);
    static TLAS CreateTLAS(const std::vector<BLAS>& allblas);
};


#endif //INSTRENDERER_RTBUILDER_H
