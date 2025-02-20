
#include "ResMaterial.h"

namespace Res
{
    void ResMaterial::AddTexture(std::shared_ptr<ResTexture> texture)
    {
        textures.push_back(texture);
    }
} // Res