
#ifndef INSTRENDERER_GAMEOBJECT_H
#define INSTRENDERER_GAMEOBJECT_H
#include "glm/glm.hpp"
#include "entt.hpp"
#include "string"
#include "vector"
#include "bitset"
#include "Component/Components.h"
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
    entt::entity entityID;
    entt::registry* reg;
    std::string name;
    int parent=-1;
    std::bitset<8> componentBits;
    std::string type = "GameObject";
};


#endif //INSTRENDERER_GAMEOBJECT_H
