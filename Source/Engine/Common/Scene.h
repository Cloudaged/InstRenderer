
#ifndef INSTRENDERER_SCENE_H
#define INSTRENDERER_SCENE_H
#include "entt.hpp"
#include "GameObject.h"
#include "MeshObject.h"
#include "vulkan/vulkan.h"
#include "vector"
#include "../Render/Buffer/Buffer.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"
struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 proj;

};

struct GlobalDescriptorData
{
    Buffer buffer;
    VkDescriptorSetLayout globalDesLayout;
    VkDescriptorSet globalDes;
};


class Scene
{
public:
    Scene();
    ~Scene();
    entt::registry reg;

    std::vector<entt::meta_any> objects;
    Camera mainCamera;
    entt::meta_any& CreateObject(std::string name,std::string type);
    entt::meta_any CreateObject(std::string name,std::string type,int parent);

    void InitGlobalSet();
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
