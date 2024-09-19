
#ifndef INSTRENDERER_IMAGELOADER_H
#define INSTRENDERER_IMAGELOADER_H
#include <stdexcept>

#include "vulkan/vulkan.h"
#include "string"
#include "ResTexture.h"
namespace Res
{

    class ImageLoader
    {
    public:
        static ResTexture* Load(std::string path);

    };

} // Res

#endif //INSTRENDERER_IMAGELOADER_H
