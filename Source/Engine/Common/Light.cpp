
#include "Light.h"

Light::Light(entt::registry *reg, std::string name,std::string type) : GameObject(reg, name,type)
{
    reg->emplace<LightComponent>(this->entityID,LightType::Directional,glm::vec3{1.0,1.0,1.0},1,1);
    this->componentBits.set((int)ComponentType::LightComponent);
}
