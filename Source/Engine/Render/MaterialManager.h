
#ifndef INSTRENDERER_MATERIALMANAGER_H
#define INSTRENDERER_MATERIALMANAGER_H
#include "vulkan/vulkan.h"
#include "entt.hpp"
#include "../Common/Component/Components.h"
#include "Material/PBRMaterial.h"
class MaterialManager
{
public:
    void Setup(VkDescriptorSetLayout* layout,entt::view<entt::get_t<Renderable>> view);
    VkDescriptorSetLayout* materialLayout;
    entt::view<entt::get_t<Renderable>> view;
private:
    void AllocateDescriptorSets();
    void AllocatePBRDes(PBRMaterial& handle);

};


#endif //INSTRENDERER_MATERIALMANAGER_H
