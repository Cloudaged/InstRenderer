
#ifndef INSTRENDERER_MESHLET_H
#define INSTRENDERER_MESHLET_H
#include "vulkan/vulkan.h"
#include "meshoptimizer.h"
struct Meshlet
{
    uint32_t vertexCount;
    uint32_t vertexOffset;
    uint32_t primitiveCount;
    uint32_t primitiveOffset;
};



#endif //INSTRENDERER_MESHLET_H
