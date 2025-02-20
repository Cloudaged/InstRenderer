\
#include "Skybox.h"

Skybox::Skybox(std::string boxPath, std::vector<std::string> paths)
{
    auto boxModel = ModelLoader::Load(boxPath);
    auto& boxMesh = boxModel->rootNode->children[0]->meshes[0];
    this->mesh = std::make_shared<Mesh>(boxMesh->verts,boxMesh->index);

    auto resTex = ImageLoader::Load(paths);

    this->width = resTex->width;
    this->height = resTex->height;
    std::shared_ptr<AllocatedImage> img = std::make_shared<AllocatedImage>(ImageType::CubeColor,VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                       VkExtent2D{(uint32_t)resTex->width,(uint32_t)resTex->height},1,paths.size());
    img->LoadData(resTex);

    this->texture = std::make_shared<Texture>(img,TextureType::DontCare);

}
