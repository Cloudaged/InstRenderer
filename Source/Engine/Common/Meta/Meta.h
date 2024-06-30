#include "entt.hpp"
#include "../GameObject.h"
#include "../MeshObject.h"
using namespace entt::literals;
static void RegisterMeta()
{
    entt::meta<GameObject>()
            .type("GameObject"_hs)
            .data<&GameObject::entityID>("entityID"_hs)
            .data<&GameObject::name>("name"_hs)
            .ctor<entt::entity,std::string>();

    entt::meta<MeshObject>()
            .type("MeshObject"_hs)
            .base<GameObject>()
            .data<&MeshObject::entityID>("entityID"_hs)
            .data<&MeshObject::name>("name"_hs)
            .data<&MeshObject::count>("count"_hs)
            .ctor<entt::entity,std::string>();
}
