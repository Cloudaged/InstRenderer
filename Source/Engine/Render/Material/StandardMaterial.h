
#ifndef INSTRENDERER_STANDARDMATERIAL_H
#define INSTRENDERER_STANDARDMATERIAL_H
#include "vulkan/vulkan.h"
#include "Material.h"
#include "vector"
#include "../Buffer/Texture.h"


struct TextureSlots
{
    int hasAlbedo=0;
    int hasNormal=0;
    int hasMetallicRoughness=0;
};

class StandardMaterial : public Material
{
public:
    StandardMaterial();
    void AddTexture(Texture* texture);
    void AddBuffer(Buffer* buffer);
    bool isSetup= false;

    //Buffer buffer;
    Buffer* slotBuffer;
    std::vector<Buffer*> buffers;
    std::vector<Texture*> textures;

};


#endif //INSTRENDERER_STANDARDMATERIAL_H
