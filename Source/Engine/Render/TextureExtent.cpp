
#include "TextureExtent.h"
#include "VulkanContext.h"
TextureExtent::TextureExtent(int width, int height):width(width),height(height)
{

}
VkExtent2D TextureExtent::GetVkExtent() const
{
    if(width==WINDOW_EXTENT.width&&height==WINDOW_EXTENT.height)
    {
        return VulkanContext::GetContext().windowExtent;
    } else
    {
        return {uint32_t(width),uint32_t(height)};
    }
}

bool TextureExtent::operator==(const TextureExtent &textureExtent) const
{
    return textureExtent.width== this->width&&textureExtent.height== this->height;
}
