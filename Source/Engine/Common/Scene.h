
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
#include "Light.h"
#include <glm/gtx/string_cast.hpp>

struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 skyboxProj;
};

struct LightUnitsInShader
{
    alignas(16) glm::vec4 position;
    alignas(16) glm::vec4 toward;
    alignas(16) glm::vec4 color;
    alignas(4) int type;
    alignas(4) float intensity;
    alignas(4) float range;
};

struct LightUniform
{
    alignas(16) glm::vec4 cameraPos;
    alignas(16) glm::vec4 cameraDir;
    alignas(4) int count;
    LightUnitsInShader lights[16];
};

struct SkyboxData
{
    Skybox* skybox;
};

struct GlobalDescriptorData
{
    Buffer globBuffer;
    Buffer lightBuffer;
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
    Light* mainLight;
    std::vector<Light*> lights;
    GameObject* CreateObject(std::string name,std::string type="GameObject");
    GameObject* CreateObject(std::string name,int parent,std::string type="GameObject");

    void InitGlobalSet();
    void InitSkyboxData();
    void InitSceneData();
    void InitMainLight();
    void UpdateAspect();

    GameObject* GetGameObject(int id);

    GlobalDescriptorData globalData;

    LightUniform lightUniform;
    GlobalUniform globUniform;
    void UpdateScene();

    template<typename T>
    void AddComponent(int objID);

    void DeleteObject(int id);
    void Destroy(int i);
    void RenameObject(int id,std::string dstName);
    void UpdateLightData();

private:
};


#endif //INSTRENDERER_SCENE_H
