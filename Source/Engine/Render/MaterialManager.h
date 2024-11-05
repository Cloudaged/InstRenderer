
#ifndef INSTRENDERER_MATERIALMANAGER_H
#define INSTRENDERER_MATERIALMANAGER_H
#include "vulkan/vulkan.h"
#include "entt.hpp"
#include "../Common/Component/Components.h"
#include "Material/StandardMaterial.h"
class MaterialManager
{
public:
    void Setup(VkDescriptorSetLayout* layout,entt::view<entt::get_t<Renderable>> view);
    VkDescriptorSetLayout* materialLayout;
    entt::view<entt::get_t<Renderable>> view;
    void AllocateDescriptorSets();
private:
    void AllocatePBRDes(StandardMaterial* handle);
    Texture* nullTexture;
    void CreateNullImg();
};


#endif //INSTRENDERER_MATERIALMANAGER_H
