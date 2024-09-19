
#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Res
{

    ResTexture *ImageLoader::Load(std::string path)
    {
        int texWidth, texHeight, texChannels;

        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture!");
        }

        ResTexture* texture =new ResTexture(texWidth, texHeight, texChannels, pixels);
        return texture;
    }
} // Res