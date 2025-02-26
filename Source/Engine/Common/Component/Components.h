
#ifndef INSTRENDERER_COMPONENTS_H
#define INSTRENDERER_COMPONENTS_H

#include "../Core/glmConfig.h"
#include "string"
#include "../../Render/Mesh.h"
#include "../../Render/Material/Material.h"
enum class ComponentType:int
{
    Transform=0,
    Renderable=1,
    LightComponent=2
};

enum class LightType:int
{
    Directional=0,
    Point=1,
    Spot = 2
};

struct Transform
{
    glm::vec3 pos = {0,0,0};
    glm::vec3 rotation = {0,0,0};
    glm::vec3 scale = {1,1,1};
    glm::mat4 globalTransform = glm::mat4 (1);
    glm::mat4 localTransform = glm::mat4 (1);
    bool isDirty = false;

};

struct Renderable
{
    Mesh mesh;
    Material material;
    Handle nodeID;
    Buffer* transformBuffer= nullptr;
};


struct ShadowCamera
{
    float nearPlane;
    float farPlane;
};

struct LightComponent
{
    LightType type= LightType::Directional;
    glm::vec3 color = {1.0,1.0,1.0};
    float Intensity =1;
    float range = 0;
    float lightSize = 300;
    float attenuation = 0;
    ShadowCamera shadowCamera={};
};

struct CameraComponent
{
    float fov;
};

#endif //INSTRENDERER_COMPONENTS_H
