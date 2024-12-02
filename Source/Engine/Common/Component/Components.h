
#ifndef INSTRENDERER_COMPONENTS_H
#define INSTRENDERER_COMPONENTS_H

#include "glm/glm.hpp"
#include "string"
#include "../../Render/Material/StandardMaterial.h"
#include "../../Render/Mesh.h"
enum class ComponentType:int
{
    Transform=0,
    Renderable=1,
    LightComponent=2
};

enum class LightType:int
{
    Directional=0,
    Point=1
};

struct Transform
{
    glm::vec3 pos = {0,0,0};
    glm::vec3 rotation = {0,0,0};
    glm::vec3 scale = {1,1,1};
};

struct Renderable
{
    Mesh* mesh;
    StandardMaterial* material;
};

struct ShadowCamera
{
    float near;
    float far;
};

struct LightComponent
{
    LightType type= LightType::Directional;
    glm::vec3 color = {1.0,1.0,1.0};
    float Intensity =1;
    float range = 0;
    ShadowCamera shadowCamera={};
};

struct CameraComponent
{
    float fov;
};

#endif //INSTRENDERER_COMPONENTS_H
