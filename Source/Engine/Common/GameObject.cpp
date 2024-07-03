
#include "GameObject.h"
#include "iostream"


GameObject::GameObject(entt::entity entity, const std::string name):entityID(entity)
{
    this->name = name;
    std::cout<<"Object Created!: "<<name<<"\n";
}
