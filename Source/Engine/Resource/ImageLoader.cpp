
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

    std::vector<unsigned char> data(pixels,pixels+imageSize);
    ResTexture* texture = new ResTexture(texWidth,texHeight,imageSize,data,path);
    return texture;
}

ResTexture *ImageLoader::Load(std::vector<std::string> path)
{
    int texWidth, texHeight, texChannels;
    VkDeviceSize totalSize=0;
    std::vector<unsigned char> data;

    for (int i = 0; i < path.size(); ++i)
    {
        stbi_uc* pixels = stbi_load(path[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        totalSize+=imageSize;
        if (!pixels)
        {
            throw std::runtime_error("failed to load texture!");
        }
        data.insert(data.end(),pixels,pixels+imageSize);
    }

    ResTexture* texture = new ResTexture(texWidth,texHeight,totalSize,data,"Skybox");
    return texture;
}
