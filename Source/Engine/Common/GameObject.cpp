
#include "GameObject.h"
#include "iostream"


GameObject::GameObject(entt::entity entity, std::string name):entityID(entity),name(name)
{
    std::cout<<"Object Created!";
}
