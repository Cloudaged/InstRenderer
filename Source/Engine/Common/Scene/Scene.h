
#ifndef INSTRENDERER_SCENE_H
#define INSTRENDERER_SCENE_H
#include "entt.hpp"
#include "GameObject.h"
#include "MeshObject.h"
#include "vulkan/vulkan.h"
#include "vector"
#include "../../Render/Buffer/Buffer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../../Resource/ImageLoader.h"
#include "../../Render/Buffer/Skybox.h"
#include "Camera.h"
#include "Light.h"
#include "glm/gtx/string_cast.hpp"
#include "SceneObserver.h"
struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 skyboxProj;
    glm::mat4 lightViewMat;
    glm::mat4 lightProjMat;
};


struct LightUnitsInShader
{
    alignas(16) glm::vec4 position;
    alignas(16) glm::vec4 toward;
    alignas(16) glm::vec4 color;
    alignas(4) int type;
    alignas(4) float intensity;
    alignas(4) float range;
    alignas(4) float outerCutoff;
    alignas(4) float attenuation;
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
    Buffer graphicBuffer;
    VkDescriptorSetLayout globalDesLayout;
    VkDescriptorSet globalDes;
    SkyboxData skyboxData;
};



using BoundingPoint = glm::vec3;

class Scene : public SceneObserver
{
public:
    Scene();
    ~Scene();
    std::shared_ptr<GameObject> CreateObject(std::string name,std::string type="GameObject");
    std::shared_ptr<GameObject> CreateObject(std::string name,std::shared_ptr<GameObject> parent,std::string type="GameObject");
    std::shared_ptr<GameObject> GetGameObject(int id);
    void UpdateScene();
    void DeleteObject(int id);
    void Destroy(int i);
    void RenameObject(int id,std::string dstName);
    void UpdateLightData();
public:
    bool isTransformDirty = true;
    std::shared_ptr<GameObject> sceneRootGameObject;
    entt::registry reg;
    std::vector<std::shared_ptr<GameObject>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    Camera mainCamera;
    std::shared_ptr<Light> mainLight;
    GlobalDescriptorData globalData{};
    LightUniform lightUniform{};
    GlobalUniform globUniform{};
    BoundingPoint minPoint;
    BoundingPoint maxPoint;
private:
    void UpdateChainedModelMatrix(std::shared_ptr<GameObject> go);
    void UpdateModelMatrix();
    void InitGlobalSet();
    void InitSkyboxData();
    void InitMainCamera();
    void InitMainLight();
    void onCameraUpdate(Camera& camera) override;
    void onLightUpdate(Light* light) override;

};


#endif //INSTRENDERER_SCENE_H
