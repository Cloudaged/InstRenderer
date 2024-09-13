
#include "GameObject.h"
#include "iostream"


GameObject::GameObject(entt::entity entity, const std::string name):entityID(entity),name(name)
{
    std::cout<<"Object Created!: "<<name<<"\n";
}

