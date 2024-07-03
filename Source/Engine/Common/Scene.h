
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

    std::vector<entt::meta_any> objects;
    entt::meta_any CreateObject(std::string name,std::string type);
    entt::meta_any CreateObject(std::string name,std::string type,int parent);
    void DeleteObject(int id);
    void Destroy(int i);
    void RenameObject(int id,std::string dstName);
};


#endif //INSTRENDERER_SCENE_H
