
#ifndef INSTRENDERER_IMAGELOADER_H
#define INSTRENDERER_IMAGELOADER_H
#include <stdexcept>

#include "vulkan/vulkan.h"
#include "string"
#include "ResTexture.h"
using namespace Res;

class ImageLoader
{
public:
    static ResTexture* Load(std::string path);
    static ResTexture* Load(std::vector<std::string> path);
};



#endif //INSTRENDERER_IMAGELOADER_H
