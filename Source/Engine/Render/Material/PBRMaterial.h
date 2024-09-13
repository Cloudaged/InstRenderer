
#ifndef INSTRENDERER_PBRMATERIAL_H
#define INSTRENDERER_PBRMATERIAL_H
#include "vulkan/vulkan.h"
#include "Material.h"
#include "vector"
#include "../Buffer/Texture.h"
class PBRMaterial : public Material
{
public:
    PBRMaterial();
    Buffer buffer;
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<Texture> textures;
    std::vector<VkDescriptorImageInfo> imageInfos;

};


#endif //INSTRENDERER_PBRMATERIAL_H
