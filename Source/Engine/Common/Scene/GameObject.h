
#ifndef INSTRENDERER_GAMEOBJECT_H
#define INSTRENDERER_GAMEOBJECT_H
#include "../Core/glmConfig.h"
#include "entt.hpp"
#include "string"
#include "vector"
#include "bitset"
#include <set>
#include "../Component/Components.h"
enum class GameObjectType
{
    GameObject,
    Light,
    Camera
};
class GameObject
{
public:
    GameObject(entt::registry* reg,std::string name,std::string type="GameObject");
public:
    entt::entity entityID;
    entt::registry* reg;
    std::string name;
    std::set<std::shared_ptr<GameObject>> child;
    std::shared_ptr<GameObject> parent;
    std::bitset<8> componentBits;
    std::string type = "GameObject";
};


#endif //INSTRENDERER_GAMEOBJECT_H
