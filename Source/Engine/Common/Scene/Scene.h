
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
public:
    std::shared_ptr<Skybox> skybox;
    std::shared_ptr<GameObject> sceneRootGameObject;
    entt::registry reg;
    std::vector<std::shared_ptr<GameObject>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    Camera mainCamera;
    std::shared_ptr<Light> mainLight;
    BoundingPoint minPoint;
    BoundingPoint maxPoint;
private:
    void InitSkyboxData();
    void InitMainCamera();
    void InitMainLight();
    void onCameraUpdate(Camera& camera) override;
    void onLightUpdate(Light* light) override;

};


#endif //INSTRENDERER_SCENE_H
