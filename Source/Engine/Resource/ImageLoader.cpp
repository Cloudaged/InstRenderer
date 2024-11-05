
#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

ResTexture *ImageLoader::Load(std::string path)
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture!");
    }

    ResTexture* texture = nullptr;
    return texture;
}
