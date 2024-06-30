
#ifndef INSTRENDERER_GAMEOBJECT_H
#define INSTRENDERER_GAMEOBJECT_H
#include "glm/glm.hpp"
#include "Component/TransformComponent.h"
#include "entt.hpp"
#include "string"
#include "vector"
class GameObject
{
public:
    GameObject(entt::entity entity,std::string name);
    entt::entity entityID;
    std::string name;
};


#endif //INSTRENDERER_GAMEOBJECT_H
