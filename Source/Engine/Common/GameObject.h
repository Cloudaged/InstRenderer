
#ifndef INSTRENDERER_GAMEOBJECT_H
#define INSTRENDERER_GAMEOBJECT_H
#include "glm/glm.hpp"
#include "entt.hpp"
#include "string"
#include "vector"
#include "bitset"
enum class GameObjectType
{
    GameObject,
    Light,
    Camera
};
class GameObject
{
public:
    GameObject(entt::registry* reg,std::string name);
    entt::entity entityID;
    entt::registry* reg;
    std::string name;
    int parent=-1;
    std::bitset<8> componentBits;
    GameObjectType type = GameObjectType::GameObject;
};


#endif //INSTRENDERER_GAMEOBJECT_H
