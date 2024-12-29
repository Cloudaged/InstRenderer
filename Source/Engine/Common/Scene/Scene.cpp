#include "Scene.h"
#include "iostream"

#include "../../Render/VulkanContext.h"
#include "../../Resource/ModelLoader.h"
Scene::Scene():
mainCamera(&reg,"mainCamera")
{
    sceneRootGameObject = CreateObject("SceneRoot");
    InitSkyboxData();
    InitMainCamera();
    InitMainLight();
}

Scene::~Scene()
{

}

std::shared_ptr<GameObject> Scene::CreateObject(std::string name,std::string type)
{
    std::shared_ptr<GameObject> go;
    if(type=="GameObject")
    {
        go = std::make_shared<GameObject>(&reg,name,type);
    }
    if(type=="Light")
    {
        auto lightGo = std::make_shared<Light>(&reg,name,type);
        lights.push_back(lightGo);
        go = std::static_pointer_cast<GameObject>(lightGo);
    }
    objects.push_back(go);

    return go;
}

std::shared_ptr<GameObject> Scene::CreateObject(std::string name, std::shared_ptr<GameObject> parent,std::string type)
{

    std::shared_ptr<GameObject> go;
    if(type=="GameObject")
    {
        go = std::make_shared<GameObject>(&reg,name,type);
        go->parent = parent;
    }
    if(type=="Light")
    {
        auto lightGo = std::make_shared<Light>(&reg,name,type);
        lightGo->parent = parent;
        lights.push_back(lightGo);
        go = std::static_pointer_cast<GameObject>(lightGo);
    }
    objects.push_back(go);

    return go;
}


void Scene::DeleteObject(int id)
{
    //Delete Child
    for (int i = 0; i < objects.size(); ++i)
    {
        std::shared_ptr<GameObject> go = objects[i];
        if ((int)go->parent->entityID==id)
        {
            Destroy(i);
        }
    }
}

void Scene::Destroy(int i)
{
    std::shared_ptr<GameObject> go =  objects[i];
    reg.destroy(go->entityID);
    objects.erase(objects.cbegin()+i);
    go.reset();
}

void Scene::RenameObject(int id, std::string dstName)
{
    for (int i = 0; i < objects.size(); ++i)
    {
        std::shared_ptr<GameObject> go =  objects[i];
        if(static_cast<int>(go->entityID)==id)
        {
            go->name = dstName;
        }

    }
}

std::shared_ptr<GameObject> Scene::GetGameObject(int id)
{
    for (auto& obj:objects)
    {
        std::shared_ptr<GameObject> go = obj;
        if(static_cast<int>(go->entityID)==id)
        {
            return go;
        }
    }
    std::cout<<"Can't find game object"<<id<<"\n";
    return nullptr;
}

void Scene::UpdateScene()
{
    mainCamera.cameraData.aspect = (float)VulkanContext::GetContext().windowExtent.width/(float)VulkanContext::GetContext().windowExtent.height;
    mainCamera.vpMat.proj = glm::perspective(mainCamera.cameraData.fov, mainCamera.cameraData.aspect,
                                             mainCamera.cameraData.nearPlane, mainCamera.cameraData.farPlane);
    mainCamera.vpMat.proj[1][1] *=-1;

  }

void Scene::InitSkyboxData()
{

    std::string boxPath = FILE_PATH("Asset/Skybox/Box/Box.gltf");

    std::string texturePath = FILE_PATH("Asset/Skybox/Textures/");

    std::vector<std::string> texPaths = {texturePath+"posx.jpg",
                                         texturePath+"negx.jpg",
                                         texturePath+"posy.jpg",
                                         texturePath+"negy.jpg",
                                         texturePath+"posz.jpg",
                                         texturePath+"negz.jpg"};

    skybox = std::make_shared<Skybox>(boxPath,texPaths);
}


void Scene::InitMainLight()
{

    mainLight = std::static_pointer_cast<Light>(CreateObject("MainLight",sceneRootGameObject,"Light"));
    sceneRootGameObject->child.insert(mainLight);
    auto& transComp = reg.get<Transform>(mainLight->entityID);
    transComp.rotation = {90,0,0};
}



void Scene::onCameraUpdate(Camera &camera)
{

    camera.vpMat.view = glm::lookAt(camera.position,
                                    camera.viewPoint+camera.position,camera.yAxis);
}

void Scene::onLightUpdate(Light* light)
{

}

void Scene::InitMainCamera()
{
    mainCamera.InitCamera(glm::vec3{0,0,0},glm::vec3{0,0,1},{0,1,0});
    mainCamera.AddObserver(this);
}





