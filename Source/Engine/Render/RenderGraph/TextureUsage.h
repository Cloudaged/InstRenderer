
#ifndef INSTRENDERER_TEXTUREUSAGE_H
#define INSTRENDERER_TEXTUREUSAGE_H
#include "vulkan/vulkan.h"

using UsageFlags = uint32_t;
namespace TextureUsage
{
  constexpr UsageFlags Unknown  = 0;
  constexpr UsageFlags ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
  constexpr UsageFlags Color = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  constexpr UsageFlags TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  constexpr UsageFlags TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  constexpr UsageFlags Present = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  constexpr UsageFlags Depth = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  constexpr UsageFlags Sampled = VK_IMAGE_USAGE_SAMPLED_BIT;
  //constexpr UsageFlags Prefiltered = 1<<6;
  constexpr UsageFlags ShadowMap = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
  constexpr UsageFlags MaterialTexture = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
  constexpr UsageFlags Storage = VK_IMAGE_USAGE_STORAGE_BIT;

}


#endif //INSTRENDERER_TEXTUREUSAGE_H
