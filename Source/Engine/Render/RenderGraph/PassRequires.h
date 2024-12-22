
#ifndef INSTRENDERER_PASSREQUIRES_H
#define INSTRENDERER_PASSREQUIRES_H
#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
using Handle = uint32_t;
struct alignas(16) GeoPC
{
    Handle baseColor;
    Handle metallicRoughness;
    Handle normal;
    glm::mat4 modelMatrix;
};

struct LightPassPC
{

};


#endif //INSTRENDERER_PASSREQUIRES_H
