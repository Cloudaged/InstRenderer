
#ifndef INSTRENDERER_LIGHT_H
#define INSTRENDERER_LIGHT_H
#include "GameObject.h"

enum class LightType
{
    Directional,
    Point
};

class Light:public GameObject
{
public:
    Light(entt::registry* reg,std::string name);
};


#endif //INSTRENDERER_LIGHT_H
