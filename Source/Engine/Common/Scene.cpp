
#include "Scene.h"
#include "iostream"
Scene::Scene()
{

}

Scene::~Scene()
{
    for (auto& obj:objects)
    {
        delete obj;
    }
}

template<typename T>
T *Scene::CreateObject(std::string name)
{
   T* obj = new T(reg.create(),name);
   return obj;
}
