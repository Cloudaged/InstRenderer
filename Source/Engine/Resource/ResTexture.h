
#ifndef INSTRENDERER_RESTEXTURE_H
#define INSTRENDERER_RESTEXTURE_H

#include "vulkan/vulkan.h"
#include "ResBase.h"
#include "vector"
enum class TextureType:uint32_t
{
    BaseColor=1,Normal,RoughnessMetallic,Emission,Ao,Roughness,Metallic,DontCare
};

namespace Res
{

    class ResTexture:public ResBase
    {
    public:
        ResTexture(int width,int height,int size,std::vector<unsigned char>,std::string path);
        int width,height,size;
        TextureType textureType=TextureType::DontCare;
        std::vector<unsigned char> data;
    };

} // Res

#endif //INSTRENDERER_RESTEXTURE_H
