
#include "StandardMaterial.h"
#include "../VulkanContext.h"
StandardMaterial::StandardMaterial()
{
    this->type = MaterialType::Pbr;

}


void StandardMaterial::AddTexture(Texture* texture)
{

    texture->TransLayout();

    textures.push_back(texture);
}
