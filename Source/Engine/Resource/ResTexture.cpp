
#include "ResTexture.h"

namespace Res
{
    ResTexture::ResTexture(int width, int height, int channel, void *data):
    width(width),height(height),channel(channel),data(data)
    {
        size = width*height*channel;
    }
} // Res