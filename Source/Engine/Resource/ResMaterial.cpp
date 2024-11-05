
#include "ResMaterial.h"

namespace Res
{
    void ResMaterial::AddTexture(ResTexture* texture)
    {
        textures.push_back(texture);
    }
} // Res