
#include "Scene.h"
#include "iostream"
Scene::Scene()
{

}

Scene::~Scene()
{

}

entt::meta_any Scene::CreateObject(std::string name,std::string type)
{
    auto type_hash = entt::hashed_string::value(type.c_str());
    auto name_hash = entt::hashed_string::value(name.c_str());

    auto type_meta = entt::resolve(type_hash);

    auto instance = type_meta.construct(reg.create(),name);

    GameObject* go= (GameObject*)instance.data();

    std::cout<<static_cast<int>(go->entityID)<<'\n';

    objects.push_back(instance);

    return instance;
}

entt::meta_any Scene::CreateObject(std::string name, std::string type, int parent)
{
    using namespace entt::literals;
    auto type_hash = entt::hashed_string::value(type.c_str());
    auto name_hash = entt::hashed_string::value(name.c_str());

    auto type_meta = entt::resolve(type_hash);
    auto parent_meta = entt::resolve(type_hash).data("parent"_hs);

    auto instance = type_meta.construct(reg.create(),name);

    parent_meta.set(instance,parent);

    GameObject* go= (GameObject*)instance.data();

    std::cout<<static_cast<int>(go->entityID)<<'\n';

    objects.push_back(instance);

    return instance;
}


void Scene::DeleteObject(int id)
{
    //Delete Child
    for (int i = 0; i < objects.size(); ++i)
    {
        GameObject* go = (GameObject*)objects[i].data();
        if (go->parent==id)
        {
            Destroy(i);
        }
    }
}

void Scene::Destroy(int i)
{
    GameObject* go =  (GameObject*)objects[i].data();
    reg.destroy(go->entityID);
    objects[i].reset();
    objects.erase(objects.cbegin()+i);
}

void Scene::RenameObject(int id, std::string dstName)
{
    for (int i = 0; i < objects.size(); ++i)
    {
        GameObject* go =  (GameObject*)objects[i].data();
        if(static_cast<int>(go->entityID)==id)
        {
            go->name = dstName;
        }

    }
}



