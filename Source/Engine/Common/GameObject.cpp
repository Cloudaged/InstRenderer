
#include "GameObject.h"
#include "iostream"


GameObject::GameObject(entt::registry* reg, const std::string name):reg(reg),name(name)
{
    entityID = reg->create();
    std::cout<<"Object Created!: "<<name<<"\n";
}

