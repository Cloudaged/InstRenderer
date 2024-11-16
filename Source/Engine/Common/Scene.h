
#ifndef INSTRENDERER_SCENE_H
#define INSTRENDERER_SCENE_H
#include "entt.hpp"
#include "GameObject.h"
#include "MeshObject.h"
#include "vulkan/vulkan.h"
#include "vector"
#include "../Render/Buffer/Buffer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../Resource/ImageLoader.h"
#include "../Render/Buffer/Skybox.h"
#include "Camera.h"
struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 skyboxProj;
};


struct SkyboxData
{
    Skybox* skybox;
};

struct GlobalDescriptorData
{
    Buffer buffer;
    VkDescriptorSetLayout globalDesLayout;
    VkDescriptorSet globalDes;
    SkyboxData skyboxData;
};

class Scene
{
public:
    Scene();
    ~Scene();
    entt::registry reg;

    std::vector<GameObject*> objects;
    Camera mainCamera;
    GameObject* CreateObject(std::string name);
    GameObject* CreateObject(std::string name,int parent);

    void InitGlobalSet();
    void InitSkyboxData();

    void InitSceneData();

    GameObject* GetGameObject(int id);

    GlobalDescriptorData globalData;

    GlobalUniform globUniform;
    void UpdateScene();

    template<typename T>
    void AddComponent(int objID);

    void DeleteObject(int id);
    void Destroy(int i);
    void RenameObject(int id,std::string dstName);
};


#endif //INSTRENDERER_SCENE_H
