
#include "GameObject.h"
#include "iostream"


GameObject::GameObject(entt::registry* reg, const std::string name,std::string type):reg(reg),name(name),type(type)
{
    entityID = reg->create();
    glm::vec3 pos = {0.0,0.0,0.0};
    reg->emplace<Transform>(entityID,pos,pos,pos);
    this->componentBits.set((int)ComponentType::Transform);
    std::cout<<"Object Created!: "<<name<<"\n";
}

