
#ifndef INSTRENDERER_SCENE_H
#define INSTRENDERER_SCENE_H
#include "entt.hpp"
#include "GameObject.h"
#include "MeshObject.h"

#include "vector"

class Scene
{
public:
    Scene();
    ~Scene();
    entt::registry reg;

    std::vector<GameObject*> objects;
    template<typename T>
    T* CreateObject(std::string name);
};


#endif //INSTRENDERER_SCENE_H
