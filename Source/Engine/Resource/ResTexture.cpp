
#include "ResTexture.h"

namespace Res
{
    ResTexture::ResTexture(std::string name,int width, int height, int size, std::vector<unsigned char> data,std::string path,int layers):
            ResBase(path,ResType::Texture),width(width),height(height),data(data),size(size),name(name),layer(layers)
    {
    }


} // Res