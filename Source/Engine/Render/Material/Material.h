
#ifndef INSTRENDERER_MATERIAL_H
#define INSTRENDERER_MATERIAL_H
#include "volk.h"
#include "vector"
using Handle = uint32_t;

struct alignas(16) Material
{
    Handle baseColor=0;
    Handle normal=0;
    Handle metallicRoughness=0;
    Handle pad0 = 0;
};



#endif //INSTRENDERER_MATERIAL_H
