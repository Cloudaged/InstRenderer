
#ifndef INSTRENDERER_MATERIAL_H
#define INSTRENDERER_MATERIAL_H
#include "vulkan/vulkan.h"
#include "vector"
#include "../RenderState.h"

enum class MaterialType
{
    Pbr
};

class Material
{
public:
    Material(){};
    VkDescriptorSet set;
    MaterialType type;
};


#endif //INSTRENDERER_MATERIAL_H
