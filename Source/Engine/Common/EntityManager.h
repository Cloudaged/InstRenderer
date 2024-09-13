
#ifndef INSTRENDERER_ENTITYMANAGER_H
#define INSTRENDERER_ENTITYMANAGER_H
#include "entt.hpp"

class EntityManager
{
public:
    EntityManager(entt::registry* registry);
    entt::registry* reg;

};


#endif //INSTRENDERER_ENTITYMANAGER_H
