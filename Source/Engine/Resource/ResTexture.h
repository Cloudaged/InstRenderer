
#ifndef INSTRENDERER_RESTEXTURE_H
#define INSTRENDERER_RESTEXTURE_H

#include "volk.h"
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
        ResTexture(std::string name,int width,int height,int size,std::vector<unsigned char>,std::string path,int layers);
        std::string name;
        int width,height,size;
        int layer = 1 ;
        TextureType textureType=TextureType::DontCare;
        std::vector<unsigned char> data;
    };

} // Res

#endif //INSTRENDERER_RESTEXTURE_H
