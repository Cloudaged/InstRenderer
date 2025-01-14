
#ifndef INSTRENDERER_IMAGELOADER_H
#define INSTRENDERER_IMAGELOADER_H
#include <stdexcept>

#include "volk.h"
#include "string"
#include "ResTexture.h"
#include "memory"
using namespace Res;

class ImageLoader
{
public:
    static std::shared_ptr<ResTexture> Load(std::string path);
    static std::shared_ptr<ResTexture> Load(std::vector<std::string> path);
};



#endif //INSTRENDERER_IMAGELOADER_H
