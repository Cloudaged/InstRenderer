
#ifndef INSTRENDERER_LIGHT_H
#define INSTRENDERER_LIGHT_H
#include "GameObject.h"
#include "Component/Components.h"

class Light:public GameObject
{
public:
    Light(entt::registry* reg,std::string name,std::string type);
};


#endif //INSTRENDERER_LIGHT_H
