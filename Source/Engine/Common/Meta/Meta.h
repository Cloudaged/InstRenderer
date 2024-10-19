#include "entt.hpp"
#include "../GameObject.h"
#include "../MeshObject.h"

#include "../Component/Components.h"
using namespace entt::literals;
static void RegisterMeta()
{
    entt::meta<GameObject>()
            .type("GameObject"_hs)
            .data<&GameObject::entityID>("entityID"_hs)
            .data<&GameObject::name>("name"_hs)
            .data<&GameObject::parent>("parent"_hs)
            .ctor<entt::registry*,std::string>();

    entt::meta<MeshObject>()
            .type("MeshObject"_hs)
            .base<GameObject>()
           /* .data<&MeshObject::entityID>("entityID"_hs)
            .data<&MeshObject::name>("name"_hs)
            .data<&MeshObject::parent>("parent"_hs)*/
            .data<&MeshObject::count>("count"_hs)
            .ctor<entt::registry*,std::string>();

    entt::meta<Transform>()
            .type("Transform"_hs)
            .data<&Transform::pos>("Position"_hs)
            .data<&Transform::rotation>("Rotation"_hs)
            .data<&Transform::scale>("Scale"_hs);
}
