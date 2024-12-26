
#ifndef INSTRENDERER_TEXTUREEXTENT_H
#define INSTRENDERER_TEXTUREEXTENT_H
#include "vulkan/vulkan.h"
#define WINDOW_EXTENT TextureExtent{-1,-1}

class TextureExtent
{
public:
    TextureExtent(int width,int height);
    bool operator==(const TextureExtent& textureExtent) const;
    VkExtent2D GetVkExtent() const;
private:
    int width;
    int height;
};


#endif //INSTRENDERER_TEXTUREEXTENT_H
