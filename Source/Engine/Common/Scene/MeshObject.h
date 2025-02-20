
#ifndef INSTRENDERER_MESHOBJECT_H
#define INSTRENDERER_MESHOBJECT_H
#include "GameObject.h"
class MeshObject : public GameObject
{
public:
    MeshObject(entt::registry* entity,std::string name);
    int count = 5;

};


#endif //INSTRENDERER_MESHOBJECT_H
