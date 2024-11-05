
#include "ResTexture.h"

namespace Res
{
    ResTexture::ResTexture(int width, int height, int size, std::vector<unsigned char> data,std::string path):
            ResBase(path,ResType::Texture),width(width),height(height),data(data),size(size)
    {
    }
} // Res